#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define FAILURE 1
#define SUCCESS 0

#ifdef _WIN32
#pragma warning (disable: 4996)
typedef unsigned char BYTE;
typedef unsigned short WORD;
#else
typedef unsigned char BYTE;
typedef unsigned int WORD;
#endif

char *tmpfname = "$tmpfil$.tmp";
char *backup_ext = ".olz";
char ipath[FILENAME_MAX];
char opath[FILENAME_MAX];
char ofname[13];

static WORD ihead[0x10];
static WORD ohead[0x10];
static WORD inf[8];
static WORD allocsize;
static long loadsize;

#ifdef _WIN32
/* mask out standart library funcs */
/* need 16bit getw/putw not 32bit */
#define getw getw2b
#define putw putw2b
int getw(FILE *fp) {
	int lo, hi;
	lo = getc(fp);
	if (lo != EOF) {
		hi = getc(fp);
		if (hi != EOF) {
			return lo | (hi << 8);
		} else {
			return EOF;
		}
	} else {
		return EOF;
	}
}
int putw(int c, FILE *fp) {
	putc((unsigned char) (c), fp);
	putc((unsigned char) (c >> 8), fp);
	return c;
}
#endif

void parsepath(char *pathname, int *fname, int *ext) {
	char c;
	int i;
	
	*fname = 0; *ext = 0;
	for (i = 0; c = pathname[i]; i++) {
		switch(c) {
			case ':':
			case '/': pathname[i] = '\\';
			case '\\':  *fname = i + 1; break;
			case '.':  *ext = i; break;
		}
	}
	if (*ext <= *fname)
		*ext = i;
}

int fnamechk(char *ipath, char *opath, char *ofname, int argc, char **argv) {
	int idx_name, idx_ext;
	
	strcpy(ipath, argv[1]);
	parsepath(ipath, &idx_name, &idx_ext);
	if (!ipath[idx_ext])
		strcpy(ipath + idx_ext, ".exe");
	if (!stricmp(ipath + idx_name, tmpfname)) {
		printf("'%s': bad filename.\n", ipath);
		return FAILURE;
	}
	if (argc == 2)
		strcpy(opath, ipath);
	else
		strcpy(opath, argv[2]);
	parsepath(opath, &idx_name, &idx_ext);
	if (!opath[idx_ext])
		strcpy(opath + idx_ext, ".ulz");
	if (!stricmp(opath + idx_ext, backup_ext)) {
		printf("'%s': bad filename.\n", opath);
		return FAILURE;
	}
	strncpy(ofname, opath + idx_name, /*12*/strlen(opath + idx_name));
	strcpy(opath + idx_name, tmpfname);
	return SUCCESS;
}

int fnamechg(char *ipath, char *opath, char *ofname, int rename_sw) {
	int idx_name, idx_ext;
	char tpath[FILENAME_MAX];
	
	if (rename_sw) {
		strcpy(tpath, ipath);
		parsepath(tpath, &idx_name, &idx_ext);
		strcpy(tpath + idx_ext, backup_ext);
		remove(tpath);
		if (rename(ipath, tpath)) {
			printf("can't make '%s'.\n", tpath);
			remove(opath);
			return FAILURE;
		}
		printf("'%s' is renamed to '%s'.\n", ipath, tpath);
	}
	strcpy(tpath, opath);
	parsepath(tpath, &idx_name, &idx_ext);
	strcpy(tpath + idx_name, ofname);
	remove(tpath);
	if (rename(opath, tpath)) {
		if (rename_sw) {
			strcpy(tpath, ipath);
			parsepath(tpath, &idx_name, &idx_ext);
			strcpy(tpath + idx_ext, backup_ext);
			rename(tpath, ipath);
		}
		printf("can't make '%s'. unpacked file '%s' is remained.\n", tpath, tmpfname);
		return FAILURE;
	}
	printf("unpacked file '%s' is generated.\n", tpath);
	return SUCCESS;
}

int rdhead(FILE *ifile , int *ver) {
	if (fread(ihead, sizeof ihead[0], 0x10, ifile) != 0x10)
		return FAILURE;
	memcpy(ohead, ihead, sizeof ihead[0] * 0x10);
	if (ihead[0] != 0x5a4d || ihead[4] != 2 || ihead[0x0d] != 0)
		return FAILURE;
	if (ihead[0x0c] == 0x1c && memcmp(&ihead[0x0e], "LZ09", 4) == 0) {
		*ver = 90;
		return SUCCESS;
	}
	if (ihead[0x0c] == 0x1c && memcmp(&ihead[0x0e], "LZ91", 4) == 0) {
		*ver = 91;
		return SUCCESS;
	}
	return FAILURE;
}

int reloc90(FILE *ifile, FILE *ofile, long fpos) {
	unsigned int c;
	WORD rel_count = 0;
	WORD rel_seg, rel_off;
	
	fseek(ifile, fpos + 0x19d, SEEK_SET);
	/* 0x19d = compressed relocation table address */
	rel_seg = 0;
	do {
		if (feof(ifile) || ferror(ifile) || ferror(ofile))
			return FAILURE;
		c = getw(ifile);
		for (; c > 0; c--) {
			rel_off = getw(ifile);
			putw(rel_off, ofile);
			putw(rel_seg, ofile);
			rel_count++;
		}
		rel_seg += 0x1000;
	} while (rel_seg != (0xf000 + 0x1000));
	ohead[3] = rel_count;
	return SUCCESS;
}

int reloc91(FILE *ifile, FILE *ofile, long fpos) {
	WORD span;
	WORD rel_count = 0;
	WORD rel_seg, rel_off;
	
	fseek(ifile, fpos + 0x158, SEEK_SET);
	/* 0x158 = compressed relocation table address */
	rel_off = 0; rel_seg = 0;
	for (;;) {
		if (feof(ifile) || ferror(ifile) || ferror(ofile))
			return FAILURE;
		if ((span = getc(ifile)) == 0) {
			span = getw(ifile);
			if (span == 0) {
				rel_seg += 0x0fff;
				continue;
			} else if (span == 1) {
				break;
			}
		}
		rel_off += span;
		rel_seg += (rel_off & ~0x0f) >> 4;
		rel_off &= 0x0f;
		putw(rel_off, ofile);
		putw(rel_seg, ofile);
		rel_count++;
	}
	ohead[3] = rel_count;
	return SUCCESS;
}
int mkreltbl(FILE *ifile, FILE *ofile, int ver) {
	long fpos;
	int i;
	
	allocsize = ((ihead[1] + 16 - 1) >> 4) + ((ihead[2] - 1) << 5) - ihead[4] + ihead[5];
	fpos = (long) (ihead[0x0b] + ihead[4]) << 4;	/* goto cs:0000 */
	fseek(ifile, fpos, SEEK_SET);
	fread(inf, sizeof inf[0], 0x08, ifile);
	ohead[0x0a] = inf[0];	/* IP */
    ohead[0x0b] = inf[1];	/* CS */
    ohead[0x08] = inf[2];	/* SP */
    ohead[0x07] = inf[3];	/* SS */
	/* inf[4]: size of compressed load module (paragraph) */
    /* inf[5]: increase of load module size (paragraph) */
    /* inf[6]: size of decompressor with  compressed relocation table (byte) */
    /* inf[7]: check sum of decompresser with compressd relocation table (ver.0.90) */
	ohead[0x0c] = 0x1c;		/* start position of relocation table */
	fseek(ofile, 0x1cL, SEEK_SET);
	switch (ver) {
		case 90: i = reloc90(ifile, ofile, fpos); break;
		case 91: i = reloc91(ifile, ofile, fpos); break;
		default: i = FAILURE; break;
	}
	if (i != SUCCESS) {
		printf("error at relocation table.\n");
		return (FAILURE);
	}
	fpos = ftell(ofile);
	i = fpos & 0x1ff;
	if (i)
		i = 0x200 - i;
	ohead[4] = (WORD) (fpos + i) >> 4;
	for ( ; i > 0; i--)
		putc(0, ofile);
	return SUCCESS;
}

typedef struct {
	FILE *fp;
	WORD buf;
	BYTE count;
} bitstream;

void initbits(bitstream *p, FILE *filep) {
	p->fp = filep;
	p->count = 0x10;
	p->buf = getw(filep);
#ifdef _DEBUG
	printf("%04x ", p->buf);
#endif
}

int getbit(bitstream *p) {
	int b;
	
	b = p->buf & 1;
	if (--p->count == 0) {
		(p->buf) = getw(p->fp);
#ifdef _DEBUG
		printf("%04x ", p->buf);
#endif
		p->count = 0x10;
	} else {
		p->buf >>= 1;
	}
	return b;
}

int unpack(FILE *ifile, FILE *ofile) {
	int len;
#ifdef _WIN32
	short int span;
#else
	int span;
#endif
	long fpos;
	bitstream bits;
	static BYTE data[0x4500];
	BYTE *p = data;

	printf(" unpacking ... ");
    fpos = (long) (ihead[0x0b] - inf[4] + ihead[4]) << 4;
	fseek(ifile, fpos, SEEK_SET);
	fpos = (long) ohead[4] << 4;
	fseek(ofile, fpos, SEEK_SET);
	initbits(&bits, ifile);
	for (;;) {
		if (ferror(ifile))
			{ printf("\nread error\n"); return FAILURE; }
		if (ferror(ofile))
			{ printf("\nwrite error\n"); return FAILURE; }
		if (p-data > 0x4000) {
			fwrite(data, sizeof data[0], 0x2000, ofile);
			p -= 0x2000;
			memcpy(data, data + 0x2000, p - data);
			putchar('.');
		}
		if (getbit(&bits)) {
			*p++ = getc(ifile);
			continue;
		}
		if (!getbit(&bits)) {
			len = getbit(&bits) << 1;
			len |= getbit(&bits);
			len += 2;
			span = getc(ifile) | 0xff00;
		} else {
			span = (BYTE) getc(ifile);
			len = getc(ifile);
			span |= ((len & ~0x07) << 5) | 0xe000;
			len = (len & 0x07) + 2;
			if (len == 2) {
				len = getc(ifile);
				if (len == 0)
					break;		/* end mark of compreesed load module */
				if (len == 1)
					continue;	/* segment change */
				else
					len++;
			}
		}
		for( ; len > 0; len--, p++)
			*p = *(p + span);
	}
	if (p != data)
		fwrite(data, sizeof data[0], p - data, ofile);
	loadsize = ftell(ofile) - fpos;
	printf("done\n");
	return SUCCESS;
}

void wrhead(FILE *ofile) {
	if (ihead[6]!= 0) {
		ohead[5] = (WORD) (allocsize - ((loadsize + 16 - 1) >> 4));
		if (ihead[6] != 0xffff)
			ohead[6] -= (ihead[5] - ohead[5]);
	}
	ohead[1] = (loadsize + (ohead[4] << 4)) & 0x1ff;
    ohead[2] = (WORD) ((loadsize + (ohead[4] << 4) + 0x1ff) >> 9);
	fseek(ofile, 0L, SEEK_SET);
	fwrite(ohead, sizeof ohead[0], 0x0e, ofile);
}

int main(int argc, char **argv) {
	FILE *ifile, *ofile;
	int ver, rename_sw = 0;

	printf("unlzexe\n");
	if (argc != 3 && argc != 2) {
		printf("usage: unlzexe packedfile [unpackedfile]\n");
		exit(EXIT_FAILURE);
	}
	if (argc == 2)
		rename_sw = 1;
	if (fnamechk(ipath, opath, ofname, argc, argv) != SUCCESS)
		exit(EXIT_FAILURE);
	if ((ifile = fopen(ipath, "rb")) == NULL) {
		printf("'%s' :not found\n", ipath);
		exit(EXIT_FAILURE);
	}
	if (rdhead(ifile, &ver) != SUCCESS) {
		printf("'%s' is not LZEXE file.\n", ipath);
		fclose(ifile);
		exit(EXIT_FAILURE);
	}
	if ((ofile = fopen(opath, "w+b")) == NULL) {
		printf("can't open '%s'.\n", opath);
		fclose(ifile);
		exit(EXIT_FAILURE);
	}
	printf("file '%s' is compressed by lzexe ver ", ipath);
	switch (ver) {
		case 90: printf("0.90\n"); break;
		case 91: printf("0.91\n"); break;
	}
	if (mkreltbl(ifile, ofile, ver) != SUCCESS) {
		fclose(ifile);
		fclose(ofile);
		remove(opath);
		exit(EXIT_FAILURE);
	}
	if (unpack(ifile, ofile) != SUCCESS) {
		fclose(ifile);
		fclose(ofile);
		remove(opath);
		exit(EXIT_FAILURE);
	}
	fclose(ifile);
	wrhead(ofile);
	fclose(ofile);
	if (fnamechg(ipath, opath, ofname, rename_sw) != SUCCESS)
		exit(EXIT_FAILURE);
	printf("\n");
	exit(EXIT_SUCCESS);
}
