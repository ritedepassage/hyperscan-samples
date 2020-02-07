//imap clinet regex: [[:alnum:][:punct:]]\\s+[\^[:alnum:][:punct:]][\\r\\n]+
//imap server capability regex: [[:alnum:][:punct:]]\\s+[\^[:alnum:][:punct:]]

#include <iostream>
#include <hs/hs.h>
#include <string.h>

//g++ --std=c++17 hs-sample.cc -lhs -o hs-s

char *data = NULL;

int match_callback(unsigned int id,
                   unsigned long long from,
                   unsigned long long to,
                   unsigned int flags,
                   void *context)
{
    printf("match callback: %llu - %llu - %s\n", from, to, data);
    return 1;
}

int main(int argc, char **argv)
{
    hs_database *db = NULL;
    hs_scratch *scratch = NULL;
    hs_compile_error *compile_error = NULL;
    // const char *expressions = {"[[:alnum:][:punct:]]"};
    const char *expressions = argv[1];
    bool append_crlf = argc == 4;

    printf("try to compile pattern: %s\n", expressions);

    if (hs_compile(expressions,
                   HS_FLAG_DOTALL | HS_FLAG_SOM_LEFTMOST,
                   HS_MODE_BLOCK,
                   NULL,
                   &db,
                   &compile_error) != HS_SUCCESS)
    {
        printf("failed to compile pattern: %s\n", compile_error->message);
        exit(-1);
    }

    hs_error_t err = hs_alloc_scratch(db, &scratch);

    if (err != HS_SUCCESS)
    {
        printf("failed to allocate scratch space: %d\n", err);
        exit(-2);
    }
    unsigned data_len = strlen(argv[2]);

    if (append_crlf)
    {
        printf("append cr lf to data\n");
        data = (char *)malloc(data_len + 3);
        memcpy(data, argv[2], data_len);
        data[data_len] = '\r';
        data[data_len + 1] = '\n';
        data[data_len + 2] = '\0';
    }
    else
    {
        data = (char *)malloc(data_len + 1);
        memcpy(data, argv[2], data_len);
        data[data_len] = '\0';
    }


    printf("try to scan on data: %s\n", data);
    err = hs_scan(db, data, strlen(data), 0, scratch, match_callback, NULL);
    if (err != HS_SUCCESS && err != HS_SCAN_TERMINATED)
    {
        printf("failed to scan data: %d\n", err);
    }

    return 0;
}
