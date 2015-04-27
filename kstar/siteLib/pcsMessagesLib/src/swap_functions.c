/* @(#)swap_functions.c	1.2 06/30/00
swap_functions.c
Functions used to change the endianness of a value.
The functions here are: 
swap_i8
swap_i4
swap_i2
*/

/*
******************************************************************************
SUBROUTINE: swap_i8

Do endian swaps in input buffer of 8 byte words.
******************************************************************************
*/
int
swap_i8(char *buffer,int numbytes)
{
    int word_start,wwrd,wbyt,word_count;
    char tmp_word8[8];

    word_count = numbytes/8;

    word_start = 0;
    for(wwrd=0;wwrd<word_count;wwrd++)
    {
        for(wbyt=0;wbyt<8;wbyt++) tmp_word8[wbyt] = buffer[word_start+wbyt];
        for(wbyt=0;wbyt<8;wbyt++) buffer[word_start+wbyt] = tmp_word8[8-wbyt-1];
        word_start = word_start + 8;
    }
    return 0;
}

/*
******************************************************************************
SUBROUTINE: swap_i4

Do endian swaps in input buffer of 4 byte words.
******************************************************************************
*/
int
swap_i4(char *buffer,int numbytes)
{
    int word_start,wwrd,wbyt,word_count;
    char tmp_word4[4];

    word_count = numbytes/4;

    word_start = 0;
    for(wwrd=0;wwrd<word_count;wwrd++)
    {
        for(wbyt=0;wbyt<4;wbyt++) tmp_word4[wbyt] = buffer[word_start+wbyt];
        for(wbyt=0;wbyt<4;wbyt++) buffer[word_start+wbyt] = tmp_word4[4-wbyt-1];
        word_start = word_start + 4;
    }
    return 0;
}

/*
******************************************************************************
SUBROUTINE: swap_i2

Do endian swaps in input buffer of 2 byte words.
******************************************************************************
*/
int
swap_i2(char *buffer,int numbytes)
{
    int word_start,wwrd,wbyt,word_count;
    char tmp_word2[2];

    word_count = numbytes/2;

    word_start = 0;
    for(wwrd=0;wwrd<word_count;wwrd++)
    {
        for(wbyt=0;wbyt<2;wbyt++) tmp_word2[wbyt] = buffer[word_start+wbyt];
        for(wbyt=0;wbyt<2;wbyt++) buffer[word_start+wbyt] = tmp_word2[2-wbyt-1];
        word_start = word_start + 2;
    }
    return 0;
}
