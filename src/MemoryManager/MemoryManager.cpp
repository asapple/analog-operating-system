#include "include/MemoryManager/MemoryManager.h"
#include "include/ProcessManager/Instruction.h"
#include <QDebug>
using namespace os;

PageTable::PageTable() :
    CODE(true), DATA(false)
{
    pid = -1; // 赋值页表所属进程
    code_now = data_now = occupy = 0;
    code_max = MEMORY_CODE_MAX;
    data_max = MEMORY_DATA_MAX;
}

PageTable::PageTable(QVector<frame_t> initpages, pid_t id) :
    CODE(true), DATA(false)
{
    pid = id; // 赋值页表所属进程
    for (int i = 0; i < MEMORY_VIRTUAL_SIZE; i++) {
        PtItem item; // 页表项
        table.insert(i, item); // 插入hash表
    }
    for (int i = 0; i < initpages.size(); i++) {
        table[i].SetFrame(initpages[i]);
        table[i].SetVi(true);
        code_queue.append(i); // 加入牺牲队列
    } // 前n个页为代码段，初始化时分配

    code_now = occupy = initpages.size();
    data_now = 0;
    code_max = MEMORY_CODE_MAX;
    data_max = MEMORY_DATA_MAX;
}

PageTable& PageTable::operator=(const PageTable& p)
{
    this->pid = p.pid;
    this->occupy = p.occupy;
    this->code_max = p.code_max;
    this->data_max = p.data_max;
    this->code_now = p.code_now;
    this->data_now = p.data_now;
    this->table = p.table;
    this->code_queue = p.code_queue;
    this->data_queue = p.data_queue;
    return *this;
}

void PageTable::AddPage(page_t vaddress, frame_t paddress)
{
    table[vaddress].SetFrame(paddress);
    table[vaddress].SetVi(true);
    occupy += 1;
}

frame_t PageTable::ReplaceCode(page_t new_vaddress)
{
    page_t out_page = code_queue[0]; // 找到牺牲页
    code_queue.pop_front(); // 从牺牲队列中将其删除
    frame_t replace_frame = table.find(out_page).value().GetFrame(); // 牺牲页的物理帧号
    table[out_page].SetVi(false); // 牺牲页失效
    table[new_vaddress].SetFrame(replace_frame); // 帧号与新虚拟页号绑定
    table[new_vaddress].SetVi(true); // 新页生效
    return replace_frame;
}

frame_t PageTable::ReplaceData(page_t new_vaddress)
{
    page_t out_page = data_queue[0]; // 找到牺牲页
    data_queue.pop_front(); // 从牺牲队列中将其删除
    frame_t replace_frame = table.find(out_page).value().GetFrame(); // 牺牲页的物理帧号
    table[out_page].SetVi(false); // 牺牲页失效
    table[new_vaddress].SetFrame(replace_frame); // 帧号与新虚拟页号绑定
    table[new_vaddress].SetVi(true); // 新页生效
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
        frame = table[vaddress].GetFrame(); // 返回该页的物理帧号
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
        frame = table[vaddress].GetFrame(); // 返回该页的物理帧号
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
            result.append(iter.value().GetFrame());
    }
    return result;
}
// TODO 文件系统完成后删除
int MemoryManager::ReadFile(const QString& file_name, QByteArray& content)
{
    content.append(Instruction(InsType::COMPUTE, 2));
    content.append(Instruction(InsType::FORK, 2));
    content.append(Instruction(InsType::COMPUTE, 3));
    content.append(Instruction(InsType::DEVICE, 3, 0));
    content.append(Instruction(InsType::PRIORITY, 2));
    content.append(Instruction(InsType::ACCESS, 33));
    content.append(Instruction(InsType::FORK, 2));
    content.append(Instruction(InsType::QUIT));
    return 0;
}

MemoryManager::MemoryManager() :
    CODE(true), DATA(false),
    memory(MEMORY_TOTAL_SIZE, '0'),
    bitmap(MEMORY_TOTAL_SIZE/MEMORY_PAGE_SIZE,-1)
     {}

MemoryManager& MemoryManager::Instance()
{
    static MemoryManager mm;
    return mm;
}

page_t MemoryManager::Virt2Page(size_t virt_addr)
{
    int page_bits = log2(MEMORY_PAGE_SIZE); // 表示页内偏移所需的位数
    QString page = QString("%1").arg(QString::number(virt_addr, 2), MEMORY_ADDR_SIZE, '0').left(MEMORY_ADDR_SIZE-page_bits);
    bool succ;
    return page.toInt(&succ, 2);
}

offset_t MemoryManager::Virt2Offset(size_t virt_addr)
{
    int page_bits = log2(MEMORY_PAGE_SIZE); // 表示页内偏移所需的位数
    QString offset = QString("%1").arg(QString::number(virt_addr, 2), MEMORY_ADDR_SIZE, '0').right(page_bits);
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
    //TODO 文件系统完善后改为
//    int error = FileManager::Instance().ReadFile(file_name, source);
    int error = ReadFile(file_name, source); // 读取文件
    if (error) return error; // 错误码不为0，上报错误
    int start = page * 8 + offset; // 计算相对与文件头的偏移量
    if (start >= source.size()) return 254; // 起始地址超出文件范围
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
        int offset = MEMORY_INSTR_SIZE * i; // 第i条指令的文件偏移量
        int page = offset / MEMORY_PAGE_SIZE; // 计算第i条指令的虚拟页号
        if (page != 0) {
            offset = offset % page; // 计算第i条指令的页内偏移
        }
        int error = ReadBytes(file_name, page, offset, MEMORY_INSTR_SIZE, content); // 从文件中复制一条指令出来
        if (error == 0) // 复制完全成功，载入内存
            CopyBytes(memory, initpages[page]*MEMORY_PAGE_SIZE+offset, content, 0, MEMORY_INSTR_SIZE);
        else if (error == 254)
            break; // 寻址地址超出文件范围
        else if (error == 255)
            return -4; // 剩余文件不足一条指令的大小
        else
            return -5; // 其他错误
    }

    PageTable pt(initpages, pid); // 初始化页表
    pt_meta.insert(pid, pt); // 在元数据中记录页表和pid的映射关系

    QString str;
    for (auto i = initpages.begin(); i != initpages.end(); i++) {
        str += QString::number(*i) + " ";
    }
    str = QString("[InitMem]  pid:%1  init frame:%2").arg(QString::number(pid), str);
    PrintInfo(str);

    return initpages.size()*MEMORY_INSTR_SIZE; // 返回成功分配的内存大小
}

int MemoryManager::GetCode(pid_t pid, size_t virt_addr, QByteArray& content)
{
    page_t page = Virt2Page(virt_addr); // 虚拟页号
    offset_t offset = Virt2Offset(virt_addr); // 页内偏移
    QString str = QString("[GetCode]  pid:%1  page:%2  offset:%3").arg(QString::number(pid), QString::number(page), QString::number(offset));
    PrintInfo(str);

    auto pt_iter = pt_meta.find(pid);
    if (pt_iter == pt_meta.end()) return -1; // 错误，该进程没有页表

    frame_t frame;
    QByteArray code(MEMORY_INSTR_SIZE ,'0');
    int visit_error = pt_iter->VisitCode(page, frame);
    str = QString("[GetCode]  pid:%1  find page:%2  result:%3").arg(QString::number(pid), QString::number(page), QString::number(visit_error));
    PrintInfo(str);
    if (visit_error == 0) { // 指令已在内存中
        CopyBytes(code, 0, memory, frame*MEMORY_PAGE_SIZE+offset, MEMORY_INSTR_SIZE);
        content = code;
    } else if (visit_error == 1) { // 指令不在内存中，已回收一个牺牲页的物理帧
        int read_error = ReadBytes(exec_name, page, offset, MEMORY_INSTR_SIZE, code);
        if (read_error == 0) {// 复制完全成功，返回指令
            CopyBytes(memory, frame*MEMORY_PAGE_SIZE+offset, code, 0, MEMORY_PAGE_SIZE); // 覆盖牺牲帧
            content = code;
        } else if (read_error == 254) {
            return -3; // 寻址地址超出文件范围
        } else if (read_error == 255) {
            return -4; // 剩余文件不足一条指令的大小
        } else {
            return -5; // 其他错误
        }
    } else if (visit_error == 2){ // 指令不在内存中，需要分配一个新的物理帧
        QVector<frame_t> new_frame;
        if (GetFrame(1, new_frame)) { // GetFrame返回1，说明内存不足
            pt_iter->SetMax(pt_iter->GetNow(CODE), CODE); // 将代码页上限设置为当前有效页数
            GetCode(pid, virt_addr, content); // 重新执行本函数
        } else { // 内存充足，调页后加入页表
            int read_error = ReadBytes(exec_name, page, offset, MEMORY_INSTR_SIZE, code);
            if (read_error == 0) {// 复制完全成功，返回指令
                bitmap[new_frame[0]] = pid; // 更改位图，标识占用
                CopyBytes(memory, new_frame[0]*MEMORY_PAGE_SIZE+offset, code, 0, MEMORY_INSTR_SIZE); // 写入新帧
                pt_iter->AddPage(page, new_frame[0]); // 将新帧加入页表
                content = code;
            } else if (read_error == 254) {
                return -3; // 寻址地址超出文件范围
            } else if (read_error == 255) {
                return -4; // 剩余文件不足一条指令的大小
            } else {
                return -5; // 其他错误
            }
        }
    }
    return 0;
}

int MemoryManager::AccessMemory(pid_t pid, size_t virt_addr)
{
    page_t page = Virt2Page(virt_addr); // 虚拟页号
    QString str = QString("[AcceMem]  pid:%1  page:%2").arg(QString::number(pid), QString::number(page));
    PrintInfo(str);

    auto pt_iter = pt_meta.find(pid);
    if (pt_iter == pt_meta.end()) return -1; // 错误，该进程没有页表

    frame_t frame;
    int visit_error = pt_iter->VisitData(page, frame);
    str = QString("[AcceMem]  pid:%1  find page:%2  result:%3").arg(QString::number(pid), QString::number(page), QString::number(visit_error));
    PrintInfo(str);

    // error == 0时，数据在内存中，直接读取，由于没有实际数据，故跳过
    // error == 1时，数据不在内存中，且需要替换页，VisitData已经完成了替换工作，只需往返回的帧号中写入新数据即可，同上跳过
    if (visit_error == 2){ // 数据不在内存中，且需要分配一个新的物理帧
        QVector<frame_t> new_frame;
        if (GetFrame(1, new_frame)) { // GetFrame返回1，说明内存不足
            size_t data_now = pt_iter->GetNow(DATA);
            if (data_now > 0) { // 当前数据页不为0
                pt_iter->SetMax(data_now, DATA); // 将数据页上限设置为当前有效页数
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

int MemoryManager::ForkMemory(pid_t pid, pid_t ppid)
{
    auto pt_iter = pt_meta.find(ppid);
    if (pt_iter == pt_meta.end()) return -1; // 错误，ppid进程没有页表
    QHash<page_t, PtItem> table_ppid = pt_iter->GetTable(); // ppid进程的页表内容
    PageTable pt_pid(QVector<frame_t>(), pid); // 创建一个空页表实例
    QVector<frame_t> new_frame;
    for (auto i = table_ppid.begin(); i != table_ppid.end(); ++i) {
        if (i->GetVi()) { // 发现已缓存的页
            if (GetFrame(1, new_frame)) return -2; // 错误，内存不足
            bitmap[new_frame.back()] = pid; // 更改位图，标识占用
            CopyBytes(memory, new_frame.back()*MEMORY_PAGE_SIZE, memory, i->GetFrame()*MEMORY_PAGE_SIZE, MEMORY_PAGE_SIZE); // 将该帧内容复制到新帧
            pt_pid.AddPage(i.key(), new_frame.back()); // 添加到新页表
        }
    }
    pt_pid.SetOccupy(pt_iter->GetOccupy());
    pt_pid.SetMax(pt_iter->GetMax(CODE), CODE);
    pt_pid.SetMax(pt_iter->GetMax(DATA), DATA);
    pt_pid.SetNow(pt_iter->GetNow(CODE), CODE);
    pt_pid.SetNow(pt_iter->GetNow(DATA), DATA);
    pt_pid.SetQueue(pt_iter->GetQueue(CODE), CODE);
    pt_pid.SetQueue(pt_iter->GetQueue(DATA), DATA);
    pt_meta.insert(pid, pt_pid);
    return 0;
}

int MemoryManager::MoreMemory(pid_t pid, size_t size)
{
    auto pt_iter = pt_meta.find(pid);
    if (pt_iter == pt_meta.end()) return -1; // 错误，该进程没有页表
    int num = ceil(size/double(MEMORY_PAGE_SIZE)); // 计算所需内存页
    pt_iter->SetMax(pt_iter->GetNow(DATA)+num, DATA); // 提高数据段页数量上限

    size_t code_now = pt_iter->GetNow(CODE), data_now = pt_iter->GetNow(DATA);
    size_t code_max = pt_iter->GetMax(CODE), data_max = pt_iter->GetMax(DATA);
    QString str = QString("[MoreMem]  pid:%1  code now:%2  code max:%3  data now:%4  data max:%5").arg(QString::number(pid), QString::number(code_now), QString::number(code_max), QString::number(data_now), QString::number(data_max));
    PrintInfo(str);

    return 0;
}

int MemoryManager::ReleaseMemory(pid_t pid)
{
    auto pt_iter = pt_meta.find(pid);
    if (pt_iter == pt_meta.end()) return -1; // 错误，该进程没有页表
    QHash<page_t, PtItem> table = pt_iter->GetTable();
    for (auto i = table.begin(); i != table.end(); i++) { // 遍历页表内容
        if (i->GetVi()) // 解除对有效页的占用
            bitmap[i->GetFrame()] = -1;
    }
//    pt_iter->~PageTable(); // 析构页表
    pt_meta.erase(pt_iter); // 删除映射
    return 0;
}

int MemoryManager::PrintOccupying(pid_t pid, QVector<frame_t>& Occupying)
{
    auto pt_iter = pt_meta.find(pid);
    if (pt_iter == pt_meta.end()) return -1; // 错误，该进程没有页表
    Occupying = pt_iter->PrintOccupying();
    return Occupying.size()*MEMORY_PAGE_SIZE; // 返回该进程所占用的内存空间大小
}

void MemoryManager::PrintInfo(QString str)
{
    bool ok = true;
    ok = false;
    if (ok) return;
    qDebug() << str.toUtf8().data();
}
