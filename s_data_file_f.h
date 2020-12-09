#ifndef S_DATA_FILE_F
#define S_DATA_FILE_F

typedef struct {
    int code_err;
    QString field_name;
    QString base_name;
    int n_rec;
    QString comment;
} s_data_file_F_err;

typedef struct {
    QString vers;
    QString packname;
    QString filename;
    QString filename_i;

    QList<s_data_file_F_err> err_list;
} s_data_file_F;

#endif // S_DATA_FILE_F

