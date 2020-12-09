#ifndef S_DATA_DOC
#define S_DATA_DOC

typedef struct {
    int id, id_doctype;
    int doc_type, status;
    bool has_org_code;
    QString org_code;
    QString doc_ser, doc_num, doc_org;
    QDate doc_date, doc_exp;
    bool has_doc_exp;
    QString doc_name, mask_ser, mask_num;
    QDate type_begin, type_end;
} s_data_doc;

#endif // S_DATA_DOC

