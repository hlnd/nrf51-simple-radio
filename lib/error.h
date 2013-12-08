#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED

#define SUCCESS 0
#define NO_MEMORY 1
#define NOT_FOUND 2

#define ASSUME_SUCCESS(ERR_CODE) \
    if (ERR_CODE != SUCCESS) \
        error_handler(ERR_CODE, __LINE__, __FILE__);

void error_handler(uint32_t err_code, uint32_t line_num, char * file_name);

#endif
