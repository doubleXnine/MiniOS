#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "global.h"
#include "proto.h"
#include "elf.h"

void read_Ehdr(u32 fd,Elf32_Ehdr *File_Ehdr,u32 offset)
{
	//modified by xw, 18/5/30
	// seek(offset);
	// read(fd,(void*)File_Ehdr,sizeof(Elf32_Ehdr));
	fake_seek(offset);
	fake_read(fd,(void*)File_Ehdr,sizeof(Elf32_Ehdr));
	//~xw
	return;
}

void read_Phdr(u32 fd,Elf32_Phdr *File_Phdr,u32 offset)
{
	//modified by xw, 18/5/30
	//seek(offset);
	// read(fd,(void*)File_Phdr,sizeof(Elf32_Phdr));
	fake_seek(offset);	
	fake_read(fd,(void*)File_Phdr,sizeof(Elf32_Phdr));
	//~xw
	return;
}

void read_Shdr(u32 fd,Elf32_Shdr *File_Shdr,u32 offset)
{
	//modified by xw, 18/5/30
	// seek(offset);
	// read(fd,(void*)File_Shdr,sizeof(Elf32_Shdr));
	fake_seek(offset);
	fake_read(fd,(void*)File_Shdr,sizeof(Elf32_Shdr));	
	return;
}

PUBLIC void read_elf(u32 fd,Elf32_Ehdr* Echo_Ehdr,Elf32_Phdr Echo_Phdr[],Elf32_Shdr Echo_Shdr[])
{
	int i;
	read_Ehdr(fd,Echo_Ehdr,0);	
	for(i = 0; i < Echo_Ehdr->e_phnum; i++)
	{
		read_Phdr(fd,&Echo_Phdr[i],Echo_Ehdr->e_phoff + Echo_Ehdr->e_phentsize * i);
	}
	for(i=0 ; i < Echo_Ehdr->e_shnum ; i++)
	{
		read_Shdr(fd,&Echo_Shdr[i],Echo_Ehdr->e_shoff + Echo_Ehdr->e_shentsize * i);
	}
	return;
}

PUBLIC void disp_Elf(Elf32_Ehdr* Echo_Ehdr,Elf32_Phdr Echo_Phdr[])
{
	int i;
	int temp;
	disp_str("Echo_Ehdr:");
	for(i = 0; i < 16; i++)
	{
		temp = (int)Echo_Ehdr->e_ident[i];
		disp_int(temp);
		disp_str(".");
	}
	disp_str("^");
	disp_int(Echo_Ehdr->e_type);
	disp_str("^");
	disp_int(Echo_Ehdr->e_machine);
	disp_str("^");
	disp_int(Echo_Ehdr->e_version);
	disp_str("^");
	disp_int(Echo_Ehdr->e_entry);
	disp_str("^");
	disp_int(Echo_Ehdr->e_phoff);
	disp_str("^");
	disp_int(Echo_Ehdr->e_shoff);
	disp_str("^");
	disp_int(Echo_Ehdr->e_flags);
	disp_str("^");
	disp_int(Echo_Ehdr->e_ehsize);
	disp_str("^");
	disp_int(Echo_Ehdr->e_phentsize);
	disp_str("^");
	disp_int(Echo_Ehdr->e_phnum);
	disp_str("^");
	disp_int(Echo_Ehdr->e_shentsize);
	disp_str("^");
	disp_int(Echo_Ehdr->e_shnum);
	disp_str("^");
	disp_int(Echo_Ehdr->e_shstrndx);

	disp_str("Echo_Phdr:");
	for(i = 0; i < 3; i++)
	{
		disp_str("PHT:");
		disp_str("^");
		disp_int(Echo_Phdr[i].p_type);
		disp_str("^");
		disp_int(Echo_Phdr[i].p_offset);
		disp_str("^");
		disp_int(Echo_Phdr[i].p_vaddr);
		disp_str("^");
		disp_int(Echo_Phdr[i].p_paddr);
		disp_str("^");
		disp_int(Echo_Phdr[i].p_filesz);
		disp_str("^");
		disp_int(Echo_Phdr[i].p_memsz);
		disp_str("^");
		disp_int(Echo_Phdr[i].p_flags);
		disp_str("^");
		disp_int(Echo_Phdr[i].p_align);
	}
	return;
}










