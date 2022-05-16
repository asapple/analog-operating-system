#include "include/MemoryManager/MemoryManager.h"
using namespace os;

PageTable::PageTable(QVector<frame_t> initpages, pid_t id)
{
    pid = id; // 赋值页表所属进程
    for (int i = 0; i < MEMORY_VIRTUAL_SIZE; i++) {
        PtItem item; // 页表项
        table.insert(i, item); // 插入hash表
        code_queue.append(i); // 加入牺牲队列
    }
    for (int i = 0; i < initpages.size(); i++) {
        table[i].SetAddr(initpages[i]);
        table[i].SetVi(true);
    } // 前n个页为代码段，初始化时分配

    code_now = occupy = initpages.size();
    data_now = 0;
    code_max = MEMORY_CODE_MAX;
    data_max = MEMORY_DATA_MAX;
}

void PageTable::AddPage(page_t vaddress, frame_t paddress)
{
    table[vaddress].SetAddr(paddress);
    table[vaddress].SetVi(true);
    occupy += 1;
}

frame_t PageTable::ReplaceCode(page_t new_vaddress)
{
    page_t out_page = code_queue[0]; // 找到牺牲页
    code_queue.pop_front(); // 从牺牲队列中将其删除
    frame_t replace_frame = table.find(out_page).value().GetAddr(); // 牺牲页的物理帧号
    table[out_page].SetVi(false); // 牺牲页失效
    table[new_vaddress].SetAddr(replace_frame); // 帧号与新虚拟页号绑定
    return replace_frame;
}

frame_t PageTable::ReplaceData(page_t new_vaddress)
{
    page_t out_page = data_queue[0]; // 找到牺牲页
    data_queue.pop_front(); // 从牺牲队列中将其删除
    frame_t replace_frame = table.find(out_page).value().GetAddr(); // 牺牲页的物理帧号
    table[out_page].SetVi(false); // 牺牲页失效
    table[new_vaddress].SetAddr(replace_frame); // 帧号与新虚拟页号绑定
    return replace_frame;
}

int PageTable::VisitCode(page_t vaddress, frame_t& frame)
{
    int result = 0;
    if (table[vaddress].GetVi() == false) { // 页表中没有找到该页
        if (code_now < code_max) { // 还有空闲帧，则上报这个结果，让上层程序去分配一个新帧
            code_now += 1;
            result = 2;
        } else { // 没有空闲帧，需要进行替换
            frame = ReplaceCode(vaddress); // 令牺牲页失效，并回收牺牲页的帧号
            result = 1;
        }
    } else { // 页表中找到了该页
        frame = table[vaddress].GetAddr(); // 返回该页的物理帧号
        result = 0;
    }
    // 找到了页则会访问，没找到则会调页后访问，最终都会更新LRU
    for (auto iter = code_queue.begin(); iter != code_queue.end(); ++iter) {
        if (*iter == vaddress) {
            iter = code_queue.erase(iter); // 删除
            break;
        }
    }
    code_queue.append(vaddress); // 加入末尾
    return result;
}

int PageTable::VisitData(page_t vaddress, frame_t& frame)
{
    int result = 0;
    if (table[vaddress].GetVi() == false) { // 页表中没有找到该页
        if (data_now < data_max) { // 还有空闲帧，则上报这个结果，让上层程序去分配一个新帧
            data_now += 1;
            result = 2;
        } else { // 没有空闲帧，需要进行替换
            frame = ReplaceData(vaddress); // 令牺牲页失效，并回收牺牲页的帧号
            result = 1;
        }
    } else { // 页表中找到了该页
        frame = table[vaddress].GetAddr(); // 返回该页的物理帧号
        result = 0;
    }
    // 找到了页则会访问，没找到则会调页后访问，最终都会更新LRU
    for (auto iter = data_queue.begin(); iter != data_queue.end(); ++iter) {
        if (*iter == vaddress) {
            iter = data_queue.erase(iter); // 删除
            break;
        }
    }
    data_queue.append(vaddress); // 加入末尾
    return result;
}

QVector<frame_t> PageTable::PrintOccupying()
{
    QVector<frame_t> result;
    for (auto iter = table.begin(); iter != table.end(); ++iter) { // 遍历页表
        if (iter.value().GetVi()) // 有效页
            result.append(iter.value().GetAddr());
    }
    return result;
}

int MemoryManager::ReadFile(const QString& file_name, QByteArray& content)
{
    content = "31512dae"
              "f55ff626"
              "5666afc5"
              "1132ace6"
              "56123548"
              "acdffcba";
    return 0;
}

MemoryManager::MemoryManager() :
    memory(MEMORY_TOTAL_SIZE, '0'),
    bitmap(MEMORY_TOTAL_SIZE/MEMORY_PAGE_SIZE,-1)
{

}

page_t MemoryManager::Virt2Page(size_t virt_addr)
{
    QString page = QString("%1").arg(QString::number(virt_addr, 2), 16, '0').left(13);
    bool succ;
    return page.toInt(&succ, 2);
}

offset_t MemoryManager::Virt2Offset(size_t virt_addr)
{
    QString offset = QString("%1").arg(QString::number(virt_addr, 2), 16, '0').right(3);
    bool succ;
    return offset.toInt(&succ, 2);
}

bool MemoryManager::GetFrame(size_t size, QVector<frame_t>& frames)
{
    size_t found = 0;
    for (int i = 0; i < bitmap.size(); i++) { // 遍历位图
        if (bitmap[i] == -1) { // 发现未被占用的物理帧
            frames.append(i);
            found++;
            if (found == size) break;
        }
    }
    if (found == size) return 0; // 物理帧充足，正确返回
    else return 1; // 内存不足，返回错误码
}

void MemoryManager::CopyBytes(QByteArray& dest, int dstart, QByteArray& sour, int sstart, size_t size)
{
    for (int i = 0; i < size; i++) {
        dest[dstart+i] = sour[sstart+i];
    }
}

int MemoryManager::ReadBytes(QString file_name, page_t page, offset_t offset, size_t size, QByteArray& content)
{
    QByteArray source;
    int error = ReadFile(file_name, source); // 读取文件
    if (error) return error; // 错误码不为0，上报错误
    int start = page * 8 + offset; // 计算相对与文件头的偏移量
    content = source.mid(start, size); // 复制
    if (start + size > source.size())
        return 255; // 复制不完全
    else
        return 0;
}

int MemoryManager::InitMemory(pid_t pid, const QString& file_name)
{
    if (pt_meta.contains(pid)) return -1; // 传入的pid已拥有页表
    exec_name = file_name; // 记录可执行文件名
    QVector<frame_t> initpages; // 申请初始页内存
    if (GetFrame(MEMORY_CODE_INIT, initpages)) return -2; // GetFrame返回1，说明内存不足
    for (auto i = initpages.begin(); i != initpages.end(); ++i) {
        bitmap[*i] = pid; // 更改位图，标识占用
    }

    QByteArray content;
    for (int i = 0; i < initpages.size(); i++) { // 往内存中载入指令
        int error = ReadBytes(file_name, i, 0, 8, content); // 从文件中复制一条指令出来
        if (error == 0) // 复制完全成功，载入内存
            CopyBytes(memory, 8*i, content, 0, 8);
        else if (error == 255)
            return -3; // 剩余文件不足8字节
        else
            return -4; // 其他错误
    }

    PageTable pt(initpages, pid); // 初始化页表
    pt_meta.insert(pid, pt); // 在元数据中记录页表和pid的映射关系
    return MEMORY_CODE_INIT*8; // 返回成功分配的内存大小
}

int MemoryManager::GetCode(pid_t pid, size_t virt_addr, QByteArray& content)
{
    page_t page = Virt2Page(virt_addr); // 虚拟页号
    offset_t offset = Virt2Offset(virt_addr); // 页内偏移

    auto pt_iter = pt_meta.find(pid);
    if (pt_iter == pt_meta.end()) return -1; // 错误，该进程没有页表

    frame_t frame;
    QByteArray code(8 ,'0');
    int error = pt_iter->VisitCode(page, frame);
    if (error == 0) { // 指令已在内存中
        CopyBytes(code, 0, memory, page*8+offset, 8);
        content = code;
    } else if (error == 1) { // 指令不在内存中，已回收一个牺牲页的物理帧
        ReadBytes(exec_name, page, offset, 8, code);
        if (error == 0) {// 复制完全成功，返回指令
            CopyBytes(memory, frame*8+offset, code, 0, 8-offset); // 覆盖牺牲帧
            content = code;
        } else if (error == 255) {
            return -3; // 剩余文件不足8字节
        } else {
            return -4; // 其他错误
        }
    } else if (error == 2){ // 指令不在内存中，需要分配一个新的物理帧
        QVector<frame_t> new_frame;
        if (GetFrame(1, new_frame)) { // GetFrame返回1，说明内存不足
            pt_iter->SetMax(pt_iter->GetCodeNum(), 0); // 将代码页上限设置为当前有效页数
            GetCode(pid, virt_addr, content); // 重新执行本函数
        } else { // 内存充足，调页后加入页表
            ReadBytes(exec_name, page, offset, 8, code);
            if (error == 0) {// 复制完全成功，返回指令
                bitmap[new_frame[0]] = pid; // 更改位图，标识占用
                CopyBytes(memory, new_frame[0]*8+offset, code, 0, 8-offset); // 写入新帧
                pt_iter->AddPage(page, new_frame[0]); // 将新帧加入页表
                content = code;
            } else if (error == 255) {
                return -3; // 剩余文件不足8字节
            } else {
                return -4; // 其他错误
            }
        }
    }
    return 0;
}

int MemoryManager::AccessMemory(pid_t pid, size_t virt_addr)
{
    page_t page = Virt2Page(virt_addr); // 虚拟页号

    auto pt_iter = pt_meta.find(pid);
    if (pt_iter == pt_meta.end()) return -1; // 错误，该进程没有页表

    frame_t frame;
    QByteArray data(8 ,'0');
    int error = pt_iter->VisitData(page, frame);

    if (error == 2){ // 数据不在内存中，且需要分配一个新的物理帧
        QVector<frame_t> new_frame;
        if (GetFrame(1, new_frame)) { // GetFrame返回1，说明内存不足
            int data_now = pt_iter->GetDataNum();
            if (data_now > 0) { // 当前数据页不为0
                pt_iter->SetMax(data_now, 1); // 将数据页上限设置为当前有效页数
                AccessMemory(pid, virt_addr); // 重新执行本函数
            } else {
                return -2; // 错误，内存不足
            }
        } else { // 内存充足，调页后加入页表
            bitmap[new_frame[0]] = pid; // 更改位图，标识占用
            pt_iter->AddPage(page, new_frame[0]); // 将新帧加入页表
        }
    }
    return 0;
}

int MemoryManager::MoreMemory(pid_t pid, size_t size)
{
    auto pt_iter = pt_meta.find(pid);
    if (pt_iter == pt_meta.end()) return -1; // 错误，该进程没有页表
    int num = ceil(size/8.0); // 计算所需内存页
    pt_iter->SetMax(pt_iter->GetDataMax()+num, 1); // 提高数据段页数量上限
    return 0;
}

int MemoryManager::ReleaseMemory(pid_t pid)
{
    auto pt_iter = pt_meta.find(pid);
    if (pt_iter == pt_meta.end()) return -1; // 错误，该进程没有页表
    pt_iter->~PageTable(); // 析构页表
    pt_meta.erase(pt_iter); // 删除映射
    return 0;
}

int MemoryManager::PrintOccupying(pid_t pid, QVector<frame_t> Occupying)
{
    auto pt_iter = pt_meta.find(pid);
    if (pt_iter == pt_meta.end()) return -1; // 错误，该进程没有页表
    Occupying = pt_iter->PrintOccupying();
    return Occupying.size()*8; // 返回该进程所占用的内存空间大小
}
