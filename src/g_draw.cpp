/*    g_draw.cpp
 *
 *    Copyright (c) 2008, eFTE SF Group (see AUTHORS file)
 *    Copyright (c) 1994-1996, Marko Macek
 *
 *    You may distribute under the terms of either the GNU General Public
 *    License or the Artistic License, as specified in the README file.
 *
 */
#ifdef  NTCONSOLE
#   define  WIN32_LEAN_AND_MEAN 1
#   include <windows.h>
#endif

#include "console.h"


int CStrLen(const char *p) {
    int len = 0, was = 0;
    while (*p) {
        len++;
        if (*p == '&' && !was) {
            len--;
            was = 1;
        }
        p++;
        was = 0;
    }
    return len;
}

#ifndef NTCONSOLE

void MoveCh(PCell B, char CCh, TAttr Attr, int Count) {
    unsigned char *p = (unsigned char *) B;
    while (Count > 0) {
        *p++ = (unsigned char) CCh;
        *p++ = (unsigned char) Attr;
        Count--;
    }
}

void MoveChar(PCell B, int Pos, int Width, const char CCh, TAttr Attr, int Count) {
    unsigned char *p = (unsigned char *) B;
    if (Pos < 0) {
        Count += Pos;
        Pos = 0;
    }
    if (Pos >= Width) return;
    if (Pos + Count > Width) Count = Width - Pos;
    if (Count <= 0) return;
    for (p += sizeof(TCell) * Pos; Count > 0; Count--) {
        *p++ = (unsigned char) CCh;
        *p++ = (unsigned char) Attr;
    }
}

void MoveMem(PCell B, int Pos, int Width, const char* Ch, TAttr Attr, int Count) {
    unsigned char *p = (unsigned char *) B;

    if (Pos < 0) {
        Count += Pos;
        Ch -= Pos;
        Pos = 0;
    }
    if (Pos >= Width) return;
    if (Pos + Count > Width) Count = Width - Pos;
    if (Count <= 0) return;
    for (p += sizeof(TCell) * Pos; Count > 0; Count--) {
        *p++ = (unsigned char)(*Ch++);
        *p++ = (unsigned char) Attr;
    }
}

void MoveStr(PCell B, int Pos, int Width, const char* Ch, TAttr Attr, int MaxCount) {
    unsigned char *p = (unsigned char *) B;

    if (Pos < 0) {
        MaxCount += Pos;
        Ch -= Pos;
        Pos = 0;
    }
    if (Pos >= Width) return;
    if (Pos + MaxCount > Width) MaxCount = Width - Pos;
    if (MaxCount <= 0) return;
    for (p += sizeof(TCell) * Pos; MaxCount > 0 && (*Ch != 0); MaxCount--) {
        *p++ = (unsigned char)(*Ch++);
        *p++ = (unsigned char) Attr;
    }
}

void MoveCStr(PCell B, int Pos, int Width, const char* Ch, TAttr A0, TAttr A1, int MaxCount) {
    unsigned char *p = (unsigned char *) B;

    char was = 0;
    if (Pos < 0) {
        MaxCount += Pos;
        Ch -= Pos;
        Pos = 0;
    }
    if (Pos >= Width) return;
    if (Pos + MaxCount > Width) MaxCount = Width - Pos;
    if (MaxCount <= 0) return;
    for (p += sizeof(TCell) * Pos; MaxCount > 0 && (*Ch != 0); MaxCount--) {
        if (*Ch == "&" && !was) {
            Ch++;
            MaxCount++;
            was = 1;
            continue;
        }
        *p++ = (unsigned char)(*Ch++);
        if (was) {
            *p++ = (unsigned char) A1;
            was = 0;
        } else
            *p++ = (unsigned char) A0;
    }
}

void MoveAttr(PCell B, int Pos, int Width, TAttr Attr, int Count) {
    unsigned char *p = (unsigned char *) B;

    if (Pos < 0) {
        Count += Pos;
        Pos = 0;
    }
    if (Pos >= Width) return;
    if (Pos + Count > Width) Count = Width - Pos;
    if (Count <= 0) return;
    for (p += sizeof(TCell) * Pos; Count > 0; Count--) {
        p++;
        *p++ = (unsigned char) Attr;
    }
}

void MoveBgAttr(PCell B, int Pos, int Width, TAttr Attr, int Count) {
    char *p = (char *) B;

    if (Pos < 0) {
        Count += Pos;
        Pos = 0;
    }
    if (Pos >= Width) return;
    if (Pos + Count > Width) Count = Width - Pos;
    if (Count <= 0) return;
    for (p += sizeof(TCell) * Pos; Count > 0; Count--) {
        p++;
        *p = ((unsigned char)(*p & 0x0F)) | ((unsigned char) Attr);
        p++;
    }
}

#else

void MoveCh(PCell B, const char *Ch, TAttr Attr, int Count) {
    PW_CHAR_INFO p = (PW_CHAR_INFO) B;
    while (Count > 0) {
		p->ucs32 = 0;
		//p->Char.UnicodeChar = Ch;
        p->Char.U8Char[0] = Ch[0];
        if( Ch[1] ) {
            p->Char.U8Char[1] = Ch[1];
            if( Ch[2] ) {
                p->Char.U8Char[2] = Ch[2];
                if( Ch[3] ) 
                    p->Char.U8Char[3] = Ch[3];
                 else p->Char.U8Char[3] = 0;
            } else p->Char.U8Char[2] = 0;
        } else p->Char.U8Char[1] = 0;
        p->Attributes = Attr;
        p++;
        Count--;
    }
}

void MoveWideCh_( PCell B, uint32_t Ch, TAttr Attr, int Count ) {
	PW_CHAR_INFO p = (PW_CHAR_INFO)B;
	while( Count > 0 ) {
		if( Ch > 0x100000 ) {
			p->ucs32 = 0xD800 | ( Ch & 0x3FF );
			p->Char.UnicodeChar = 0xD800 | ( (Ch>>10) & 0x3FF );
		}
		else {
			p->ucs32 = 0;
			p->Char.UnicodeChar = Ch;
		}
		p->Attributes = Attr;
		p++;
		Count--;
	}
}

void MoveChar(PCell B, int Pos, int Width, const char *Ch, TAttr Attr, int Count) {

    PW_CHAR_INFO p = (PW_CHAR_INFO) B;
    if (Pos < 0) {
        Count += Pos;
        Pos = 0;
    }
    if (Pos >= Width) return;
    if (Pos + Count > Width) Count = Width - Pos;
    if (Count <= 0) return;
    for (p += Pos; Count > 0; Count--) {
        p->Char.U8Char[0] = Ch[0];
        if( Ch[1] ) {
            p->Char.U8Char[1] = Ch[1];
            if( Ch[2] ) {
                p->Char.U8Char[2] = Ch[2];
                if( Ch[3] ) 
                    p->Char.U8Char[3] = Ch[3];
                else
                    p->Char.U8Char[3] = 0;
            }
            else
                p->Char.U8Char[2] = 0;
        }
        else
            p->Char.U8Char[1] = 0;
        p->Attributes = Attr;
        p++;
    }
}

void MoveMem(PCell B, int Pos, int Width, const char* Ch, TAttr Attr, int Count) {
    PW_CHAR_INFO p = (PW_CHAR_INFO) B;

    if (Pos < 0) {
        Count += Pos;
        Ch -= Pos;
        Pos = 0;
    }
    if (Pos >= Width) return;
    if (Pos + Count > Width) Count = Width - Pos;
    if (Count <= 0) return;
    for (p += Pos; Count > 0; Count--) {
        p->Char.UnicodeChar = (unsigned char)*Ch++;
        p->Attributes = Attr;
        p++;
    }
}

void MoveStr(PCell B, int Pos, int Width, const char* Ch, TAttr Attr, int MaxCount) {
    PW_CHAR_INFO p = (PW_CHAR_INFO) B;

    if (Pos < 0) {
        MaxCount += Pos;
        Ch -= Pos;
        Pos = 0;
    }
    if (Pos >= Width) return;
    if (Pos + MaxCount > Width) MaxCount = Width - Pos;
    if (MaxCount <= 0) return;
    for( p += Pos; MaxCount > 0 && ( *Ch != 0 ); MaxCount-- ) {
        int ch = ( p->Char.U8Char[0] = *Ch++ ) & 0xF0;
        if( ch & 0x80 ) {
            p->Char.U8Char[1] = *Ch++;
            if( ( ch & 0x20 ) ) {
                p->Char.U8Char[2] = *Ch++;
                if( ( ch & 0x10 ) ) {
                    p->Char.U8Char[3] = *Ch++;
                } else
                    p->Char.U8Char[3] = 0;
            } else
                p->Char.U8Char[2] = 0;
        } else
            p->Char.U8Char[1] = 0;
        //p->Char.UnicodeChar = (unsigned char)*Ch++;
        p->Attributes = Attr;
        p++;
    }
}

void MoveCStr(PCell B, int Pos, int Width, const char* Ch, TAttr A0, TAttr A1, int MaxCount) {
    PW_CHAR_INFO p = (PW_CHAR_INFO) B;
    char was;
    //TAttr A;

    if (Pos < 0) {
        MaxCount += Pos;
        Ch -= Pos;
        Pos = 0;
    }
    if (Pos >= Width) return;
    if (Pos + MaxCount > Width) MaxCount = Width - Pos;
    if (MaxCount <= 0) return;
    was = 0;
    for (p += Pos; MaxCount > 0 && (*Ch != 0); MaxCount--) {
        if (*Ch == '&' && !was) {
            Ch++;
            MaxCount++;
            was = 1;
            continue;
        }
        p->Char.U8Char[0] = (unsigned char)( *Ch++ );
        {
            int n = 1;
            while( ( Ch[1] & 0xC0 ) == 0x80 ) {
                p->Char.U8Char[n++] = Ch[1];
                Ch++;
            }
            p->Char.U8Char[n] = 0;
        }
        if (was) {
            p->Attributes = A1;
            was = 0;
        } else
            p->Attributes = A0;
        p++;
    }
}

void MoveAttr(PCell B, int Pos, int Width, TAttr Attr, int Count) {
    PW_CHAR_INFO p = (PW_CHAR_INFO) B;

    if (Pos < 0) {
        Count += Pos;
        Pos = 0;
    }
    if (Pos >= Width) return;
    if (Pos + Count > Width) Count = Width - Pos;
    if (Count <= 0) return;
    for (p += Pos; Count > 0; Count--, p++)
        p->Attributes = Attr;
}

void MoveBgAttr(PCell B, int Pos, int Width, TAttr Attr, int Count) {
    PW_CHAR_INFO p = (PW_CHAR_INFO) B;

    if (Pos < 0) {
        Count += Pos;
        Pos = 0;
    }
    if (Pos >= Width) return;
    if (Pos + Count > Width) Count = Width - Pos;
    if (Count <= 0) return;
    for (p += Pos; Count > 0; Count--) {
        p->Attributes =
            ((unsigned char)(p->Attributes & 0xf)) |
            ((unsigned char) Attr);
        p++;
    }
}

#endif
