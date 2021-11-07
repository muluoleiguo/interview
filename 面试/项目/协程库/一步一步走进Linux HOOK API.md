### 一、ELF

首先今天主要带大家认识的是ELF文件,在Linux中,ELF文件主要是应用在可执行文件,重定位文件,可执行文件动态连接库。首先来看一下ELF Head的定义:

```c
#define EI_NIDENT (16)
 
 
typedef struct
 
{
 
  unsigned char e_ident[EI_NIDENT]; /* Magic number and other info */
 
  Elf32_Half e_type; /* Object file type */
 
  Elf32_Half e_machine; /* Architecture */
 
  Elf32_Word e_version; /* Object file version */
 
  Elf32_Addr e_entry; /* Entry point virtual address */
 
  Elf32_Off e_phoff; /* Program header table file offset */
 
  Elf32_Off e_shoff; /* Section header table file offset */
 
  Elf32_Word e_flags; /* Processor-specific flags */
 
  Elf32_Half e_ehsize; /* ELF header size in bytes */
 
  Elf32_Half e_phentsize; /* Program header table entry size */
 
  Elf32_Half e_phnum; /* Program header table entry count */
 
  Elf32_Half e_shentsize; /* Section header table entry size */
 
  Elf32_Half e_shnum; /* Section header table entry count */
 
  Elf32_Half e_shstrndx; /* Section header string table index */
 
} Elf32_Ehdr;
 
```

**e_ident:** 这个成员,是ELF文件的第一个成员,该成员是个数字,根据上面的宏可以看出,这个程序是个16字节的数据.该成员的前4个字节依次是 0x7F,0x45,0x4c,0x46,也 就是"\177ELF"。这是ELF文件的标志，任何一个ELF文件这四个字节都完全相同。

**e_type**:          这个成员是ELF文件的类型:

                         1:表示此文件是重定位文件.
    
                         2:表示可执行文件.
    
                         3:表示此文件是一个动态连接库。

**e_machine**:       这个成员表示机器版本.具体定义参与elf.h (篇幅问题,太长了)

**e_version**:          这个成员表示ELF文件版本,为 1

**e_entry**:              这个成员表示可执行文件的入口虚拟地址。此字段指出了该文件中第一条可执 行机器指令在进程被正确加载后的内存地址！ELF可执行文件只能被加载到固定位 置.

**e_phoff**:             这个成员表示程序头(Program Headers)在ELF文件中的偏移量。如果程序头 不存在此值为0。

**e_shoff**:             这个成员表示节头(Section Headers:)在ELF文件中的偏移量。如果节头不存 在此值为0。

**e_flags**:             这个成员表示处理器标志.

**e_ehsize**:          这个成员描述了“ELF头”自身占用的字节数。

**e_phentsize**:    该成员表示程序头中的每一个结构占用的字节数。程序头也叫程序头表，可以 被看做一个在文件中连续存储的结构数组，数组中每一项是一个结构，此字段 给出了这个结构占用的字节大小。

**e_phoff**:             指出程序头在ELF文件中的起始偏移。

**e_phnum**:        此字段给出了程序头中保存了多少个结构。如果程序头中有3个结构则程序头 在文件中占用了3×e_phentsize个字节的大小。

**e_shentsize**:   节头中每个结构占用的字节大小。节头与程序头类似也是一个结构数组，关于 这两个结构的定义将分别在讲述程序头和节头的时候给出。

**e_shnum**:        节头中保存了多少个结构。

**e_shstrndx**:     这是一个整数索引值。节头可以看作是一个结构数组，用这个索引值做为此数 组的下标，它在节头中指定的一个结构进一步给出了一个“字符串表”的信息，而这 个字符串表保存着节头中描述的每一个节的名称，包括字符串表自己也是其中的一 个节。



### 二、程序头

```c
typedef struct
 
{
 
   Elf32_Word p_type; /* Segment type */
 
   Elf32_Off p_offset; /* Segment file offset */
 
   Elf32_Addr p_vaddr; /* Segment virtual address */
 
   Elf32_Addr p_paddr; /* Segment physical address */
 
   Elf32_Word p_filesz; /* Segment size in file */
 
   Elf32_Word p_memsz; /* Segment size in memory */
 
   Elf32_Word p_flags; /* Segment flags */
 
   Elf32_Word p_align; /* Segment alignment */
 
} Elf32_Phdr;
```

p_type:            段的类型

在elf.h头文件中,有很详细的说明段的类型,比如PT_LOAD 表示加载的程序段

p_offset:        文件偏移

该段在文件中的偏移。这个偏移是相对于整个文件的。

p_vaddr:         加载后的虚拟地址

该段加载后在进程空间中占用的内存起始地址。

p_paddr:        该段的物理地址

这个字段被忽略，因为在多数现代操作系统下物理地址是进程无法触及的。

p_filesz:         该段在文件中占用的字节大小

有些段可能在文件中不存在但却占用一定的内存空间，此时这个字段为0。

p_memsz:     该段在内存中占用的字节大小

有些段可能仅存在于文件中而不被加载到内存，此时这个字段为0。

p_flags:         段的属性

表示该段的读写执行等属性.elf.h文件中的定义是

**p_align:**     对齐

现代操作系统都使用虚拟内存为进程序提供更大的空间，分页技术功不可没，页就成了最小的内存分配单位，不足一页的按一页算。所以加载程序数据一般也从一页的起始地址开始，这就属于对齐。



### 三、ELF节头

天主要讲述的是ELF 文件的另一个大块叫节头(Section Headers),那么什么叫节头呢.节头里面分散着不同的段,有.bss .text .init .data .got .shstrtab 等等.节头表主要是在连接的角度看待ELF的.每一个节都保存着该节特有的数据,由于节中数据的用途不同,节被分为不同的类型,每种类型都又自己组织数据的方式.有的节储存着一些字符串,例如前面提过的.shstrtab 这个节,就专门储存节对应名词的字符串.今天这一节内容,我们就用使用这个节,获取节的名字.

下面按照我们每一次的惯例,先看一下结构体说明,然后再逐一介绍每一个成员,在前面几节中,我们主要是对ELF进行一个全面的了解,为以后进行HOOK做充分的准备.因为那部分得在ELF文件的基础上去实施的,好了.加油吧~~~


```c
/* Section header.  */
typedef struct
{
  Elf32_Word	sh_name;	 /* Section name (string tbl index) */
  Elf32_Word	sh_type;	 /* Section type */
  Elf32_Word	sh_flags;	 /* Section flags */
  Elf32_Addr	sh_addr;	 /* Section virtual addr at execution */
  Elf32_Off	sh_offset;	 /* Section file offset */
  Elf32_Word	sh_size;	 /* Section size in bytes */
  Elf32_Word	sh_link;	 /* Link to another section */
  Elf32_Word	sh_info;	 /* Additional section information */
  Elf32_Word	sh_addralign;	 /* Section alignment */
  Elf32_Word	sh_entsize;	 /* Entry size if section holds table */
} Elf32_Shdr;
```

**sh_name**: 该成员是字符串表中的一个索引

通过该索引就可以获取这个节所对应的节的名字.通常同节的类型(sh_type)来判断当前是属于什么节.

**sh_type**: 节的类型

这个成员可以告诉我们这个节里面存放的到底是什么数据,对于不同的数据,我们再通过不同的数据结构去获取数据

**sh_flags**: 节的属性

这个成员指定该节的内容是否可读可写等属性,具体定义如下:
sh_addr: 指向进程空间内的虚拟地址

如果这个节需要被加载到进程中,那么该字段就指向内存中的虚拟地址



**sh_offset**:  指向文件空间内的绝对偏移(相对整个文件的起始地址的)

该地址可以获取到很多关于节内容的信息.



**sh_size**: 节的内容大小

如果此节在文件中占用一定的字节,那么这个字段给出了该节所占用的字节大小,

如果此节不存在于文件却在内存中,那么这个字段给出了该节在内存中的字节大小



**sh_link**:  如果这个节于别的节相连,那么这个字段给出了相关的节在节头中的索引



**sh_info**: 额外的信息

对于部分节来说,还有一些额外的信息.



**sh_addralign**:  地址对齐

对于双字节等数据格式,很多时候必须对数据进行必要的对齐,来保证数据的准确存储,这个数是2的整数次幂,对齐只能有2字节对齐,4字节对齐,8字节对齐等,如果是0或者1表示这个节不用对齐.



**sh_entsize**: 指定节内部数据的大小

这个字段代表字节大小的数,对于某些字节才有意义,例如动态符号节来说,这个字段就给出动态符号表中每一个符号结构的字节大小.



上面说到,要获取节的名字要怎么做呢.sh_name字段只是保存了一个值,并不是字符串地址,那要怎么才能获取字符串呢.在前面讲述的第一个ELF Head的时候有这样一个字段e_shstrndx这个成员就是指向字符串表的索引,就可以知道找到节点中的字符串节表了.

同样在遍历节头的时候,如果字段类型等于SHT_STRTAB,那么对应的也就是字符串表.但是为了方便,在ELF Head中已经指出了他所对应的索引了.在字符串表中是以一系列的'\0'结尾的字符串,在这个节的第一个字节也是0,为什么第一个字节会是0呢,其实这就是空字符串.在这个节的最后一个字节也是0. 因为字符串的最后一个都市'\0'.

既然找到了字符串表,那怎么获取他里面的名字呢,其实可以把字符串表当成一个很大的char型数组sh_name就是所需要的字符串的首字符,通过字符串表首地址加上相对的偏移量就是对应的字符串的名字了.


### 四、符号节

这一节主要是讲述的是符号节.要怎么才能找到符号节呢,其实只要在上一期讲的遍历节头的时候,判断每一个节类型是不是SHT_SYMTAB或SHT_DYNSYM,那么相应的节就是一个符号节了,符号节存放的是一张符号表,符号表也是一个连续存储的结构数组.

那什么叫符号呢?编程过程中用到的变量和函数都可以称之为符号,一个ELF文件中并不只有一个符号节,通常是两个,一个叫动态节,打印名称为".dynsym",它的类型是SHT_DYNSYM,所有引入的外部符号在这里有所体现.另一个类型值为SHT_SYMTAB,名字为".symtab",它保存了所有有用的符号信息.


```c
typedef struct
{
  Elf32_Word	st_name;	 /* Symbol name (string tbl index) */
  Elf32_Addr	st_value;	 /* Symbol value */
  Elf32_Word	st_size;	 /	* Symbol size */
  unsigned char	st_info;	 /* Symbol type and binding */
  unsigned char	st_other;	 /* Symbol visibility */
  Elf32_Section	st_shndx;	 /* Section index */
} Elf32_Sym;
```

st_name; 符号的名字索引

这个成员是一个符号表对应的字符串表中的一个偏移,对应的字符串表可以通过该节link成员获取得到.

st_value; 符号地址

该成员给出了相应的符号值,根据符号类型和用途,它可能是个内存地址或者绝对值

st_size; 符号的字节大小

如果是数据对象,可能给出该数据对象占用的大小,如果是函数,可能给出了函数指令占用的大小.

st_info; 符号的信息

这个成员有2部分组成,第四位用来表示符号的类型.高四位表示这个符号的绑定类型,对于从动态库引入的函数,这个字段就应该为STB_GLOBAL,表示这个符号是全局符号.在elf.h中,给出了如下2个宏,用来获取符号信息


***st_other;*** 此字段恒为0

***st_shndx;*** 每个符号和某些节相关,这个字段给出了一个节头的索引.如果函数体所在的节不存于当前文件中,此字段为0.



### 五、实战

这一讲中,我们不在继续进入研究动态符号表的获取了,今天咱们来点实战的话题,对于前面我们研究的那么多知识,做一次总结.来看看这些节表之间是怎么联系起来的.最后我们将经过一个简单的拦截printf函数,修改它的参数,来输出我指定的字符串,这样就达到简单的hook api的目的.好了废话不多说.先看看main.c里面的代码.

```c
#include <stdio.h>
#include <fcntl.h>
int main(){
while(1){
getchar();
printf("hello");
fflush(stdout);
}
return 0;
}
```

这段代码很简单,敲一个回车就输出一个hello,我们的目的就是实现,敲一个hello让他输出我们指定的字符串.

首先先运行这个main程序.在另一个终端里面输入ps -a 查看main 的进程ID.一会我们用GDB手动修改的时候要attach 连接他的进程 ID.

```
$ ps -a

  PID TTY          TIME CMD
..............
 6275 pts/4    00:00:00 main
..............
```



这里是获取main 程序的进程空间的映射,找到未被使用的空间来写入我们指定的字符串,这里使用的指令是:

```
cat /proc/6275/maps

00258000-00274000 r-xp 00000000 08:01 918417     /lib/i386-linux-gnu/ld-2.13.so

00274000-00275000 r--p 0001b000 08:01 918417     /lib/i386-linux-gnu/ld-2.13.so

00275000-00276000 rw-p 0001c000 08:01 918417     /lib/i386-linux-gnu/ld-2.13.so

0058f000-00590000 r-xp 00000000 00:00 0          [vdso]

00cf6000-00e50000 r-xp 00000000 08:01 918430     /lib/i386-linux-gnu/libc-2.13.so

00e50000-00e51000 ---p 0015a000 08:01 918430     /lib/i386-linux-gnu/libc-2.13.so

00e51000-00e53000 r--p 0015a000 08:01 918430     /lib/i386-linux-gnu/libc-2.13.so

00e53000-00e54000 rw-p 0015c000 08:01 918430     /lib/i386-linux-gnu/libc-2.13.so

00e54000-00e57000 rw-p 00000000 00:00 0 

08048000-08049000r-xp00000000 08:01 1158586    /home/Linux_Project/ELF/Hello/main

08049000-0804a000rw-p0000000008:01 1158586    /home/Linux_Project/ELF/Hello/main

b7723000-b7724000 rw-p 00000000 00:00 0 

b7733000-b7736000 rw-p 00000000 00:00 0 

bfa4c000-bfa6d000 rw-p 00000000 00:00 0          [stack]
```

这里我们选用00e54000-00e57000 rw-p 00000000 00:00 0 在下面我们会向这个地址空间.写入我们需要的字符串.

 

这里我们上面只是做了个提前预报会使用这个来获取未使用空间,下面我们开始通过手动方式来拦截printf这个函数的参数.

启动我们的调试器GDB

# gdb

```
GNU gdb (Ubuntu/Linaro 7.2-1ubuntu11) 7.2
...................
```

通过attach之类依附到已经运行的进程中去,这里我们依附到main程序中去.

```
(gdb) attach 6275

Attaching to process 6275

Reading symbols from /home/Linux_Project/ELF/Hello/main...(no debugging symbols found)...done.

Reading symbols from /lib/i386-linux-gnu/libc.so.6...(no debugging symbols found)...done.

Loaded symbols for /lib/i386-linux-gnu/libc.so.6

Reading symbols from /lib/ld-linux.so.2...(no debugging symbols found)...done.

Loaded symbols for /lib/ld-linux.so.2

0x0058f416 in __kernel_vsyscall ()
```

首先万事都是从main开始,所以先查看main函数处的代码,距离我们的目标又近了一步,看到printf函数的调用的地方了.根据源代码的分析,这里是唯一一个使用printf的函数调用的地方.

```
(gdb) disassemble main

Dump of assembler code for function main:

   0x08048404 <+0>: lea     0x4(%esp),%ecx

   0x08048408 <+4>: and     $0xfffffff0,%esp

   0x0804840b <+7>: pushl   -0x4(%ecx)

   0x0804840e <+10>: push    %ebp

   0x0804840f <+11>: mov     %esp,%ebp

   0x08048411 <+13>: push    %ecx

   0x08048412 <+14>: sub     $0x4,%esp

   0x08048415 <+17>: call    0x80482e0 <getchar@plt>

   0x0804841a <+22>: movl    $0x8048510,(%esp)

   0x08048421 <+29>: call    0x8048320 <printf@plt>

   0x08048426 <+34>: mov     0x8049620,%eax

   0x0804842b <+39>: mov     %eax,(%esp)

   0x0804842e <+42>: call    0x8048310 <fflush@plt>

   0x08048433 <+47>: jmp     0x8048415 <main+17>

End of assembler dump.
```

根据ELF文件格式,printf跳转的地址是PLT表内,所以查看PLT内的printf地址处的代码,可见这里jmp跳转的地址就是根据前面介绍的GOT表了.那么理论上,就应该跳转到printf函数内去执行了.但是其实不是的.linux采用了一个叫"懒模式"的加载方式,当某个函数第一次被调用,它才会把真正的地址放到GOT表内去.

```
(gdb) x/10i 0x8048320

   0x8048320 <printf@plt>: jmp    *0x8049618

   0x8048326 <printf@plt+6>: push   $0x20

   0x804832b <printf@plt+11>: jmp    0x80482d0

   0x8048330 <_start>: xor    %ebp,%ebp

   0x8048332 <_start+2>: pop    %esi

   0x8048333 <_start+3>: mov    %esp,%ecx

   0x8048335 <_start+5>: and    $0xfffffff0,%esp

   0x8048338 <_start+8>: push   %eax

   0x8048339 <_start+9>: push   %esp

   0x804833a <_start+10>: push   %edx
```

所以我们查看下GOT表内的数据,发现这个地址很熟悉,这个0x08048326不正是plt表内的JMP下面的地址嘛.由此可见,当第一次加载函数的时候,其实GOT表内还没有得到真正的函数地址,而是返回去支持PLT表内下面的之类,push跟jmp 然后push会把printf函数的相关偏移数据送入栈,jmp去寻找这个函数的地址,然后修改got表对应的地址处的值,这样,当下次再调用printf的时候,那么GOT表内才是printf真正的函数地址

```
(gdb) x/10xw 0x8049618

0x8049618 <_GLOBAL_OFFSET_TABLE_+28>: 0x08048326 0x00000000 0x00e534e0 0x00000000
```

首先我们对plt表内的0x8048320地址下一个断点

```
(gdb) x/10b 0x8048320

0x8048320 <printf@plt>: 0xcc 0x00 0x00 0x00 0x04 0x08 0x68 0x20

0x8048328 <printf@plt+8>: 0x00 0x00
```

然后让程序继续执行.

```
(gdb) c

Continuing.
```

当你在main程序内输入一个回车的时候,GDB就会收到信号通知,表示我们下的断点起作用了.程序调用了printf函数.

```
Program received signal SIGTRAP, Trace/breakpoint trap.

0x08048321 in printf@plt ()
```

这里我们先恢复被修改的值,改回原来的指令

```
(gdb) set *(0x8048320)=0x961825ff
```

这里查看下eip的值,把eip指向`0x8048320 <printf@plt>: jmp    *0x8049618`

这条指令

```
(gdb) info register eip

eip            0x8048321 0x8048321 <printf@plt+1>

(gdb) set $eip=0x8048320

(gdb) info register eip

eip            0x8048320 0x8048320 <printf@plt>

(gdb) x/10b 0x8048320

0x8048320 <printf@plt>: 0xff 0x25 0x18 0x96 0x04 0x08 0x68 0x20

0x8048328 <printf@plt+8>: 0x00 0x00
```

接着.我们重新说明下我们此行的目的,就是为了修改printf的参数地址,使其志向我们给定的字符串地址,用于替换它原本的输出字符串.那么首先得查看下他的栈内的参数情况.

```
(gdb) info register esp

esp            0xbfa6b0ac  0xbfa6b0ac
```

我们发现当前的esp指向的是0xbfa6b0ac 继续查看0xbfa6b0ac 处的数据

```
(gdb) x/10xw 0xbfa6b0ac

0xbfa6b0ac: 0x08048426 0x08048510 0xbfa6b0d0 0xbfa6b148

0xbfa6b0bc: 0x00d0ce37 0x08048450 0x00000000 0xbfa6b148

0xbfa6b0cc: 0x00d0ce37 0x00000001  

0xbfa6b0ac = 0x08048426
```

0x08048426 熟悉不????这不就是我们main函数里面的printf下面的一条指令嘛,那么这应该就是call调用的时候留下的call指令反弹地址.调用完printf之后返回的指令地址,那么再查看0xbfa6b0b0处的值0x08048510也刚好就是我们printf参数的地址.

这样一来,我们的目的就很明确了修改0xbfa6b0ac 地址处的值,指向我们新的字符串地址,那么任务就完成了.

首先找到被调试程序的内存内是否有空余的空间,这个在前面已经讲过了.这里我们选用0x08048510这个内存空间.如下,添加了字符串(我不太熟悉gdb只好用笨方法添加了)

先修改参数地址.

```
(gdb) set *(0xbfa6b0b0)=0x00e54000

(gdb) x/10xw 0xbfa6b0ac

0xbfa6b0ac: 0x08048426 0x00e54000 0xbfa6b0d0 0xbfa6b148

0xbfa6b0bc: 0x00d0ce37 0x08048450 0x00000000 0xbfa6b148

0xbfa6b0cc: 0x00d0ce37 0x00000001
```

写入字符串

```
(gdb) set *(0x00e54000)=0x62696c2f

(gdb) set *(0x00e54000)=0x2d646c2f

(gdb) set *(0x00e54000)=0x62696c2f

(gdb) set *(0x00e54004)=0x2d646c2f

(gdb) set *(0x00e54008)=0x756e696c

(gdb) set *(0x00e5400c)=0x6f732e78

(gdb) set *(0x00e54010)=0x0000322e

(gdb) x/s 0xe54000

0xe54000:  "/lib/ld-linux.so.2"

(gdb) c

Continuing.
```

# ./main

/lib/ld-linux.so.2注意这里,已经输出了我们指定的字符串,初步已经达到了我们的目的.

设想.如果我们写入一段修改esp内printf函数参数的汇编代码嵌入到对方进程内,然后修改PLT的第一条指令,使其跳至我们的代码里,然后再修改完之后跳回GOT表内,那么main这个程序就永远不会打印hello字符串了.

好了,就说到这了.教程写的不是太好.我已经尽量将我能表达的都表达到了.要是有那里不太清楚了,希望大家可以留言,我会第一时间给你答复的.

### 七、

https://blog.csdn.net/yyttiao/article/details/7423549?utm_medium=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7Edefault-11.no_search_link&depth_1-utm_source=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7Edefault-11.no_search_link