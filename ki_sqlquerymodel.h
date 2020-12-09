#ifndef KI_SQLQUERYMODEL
#define KI_SQLQUERYMODEL

#include <QSqlQueryModel>

class ki_persSqlQueryModel : public QSqlQueryModel {
public:
    ki_persSqlQueryModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const;
};

class ki_docsSqlQueryModel : public QSqlQueryModel {
public:
    ki_docsSqlQueryModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const;
};

class ki_polSqlQueryModel : public QSqlQueryModel {
public:
    ki_polSqlQueryModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const;
};

class ki_calendarSqlQueryModel : public QSqlQueryModel {
public:
    ki_calendarSqlQueryModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const;
};

class ki_names2sexSqlQueryModel : public QSqlQueryModel {
public:
    ki_names2sexSqlQueryModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const;
};

class ki_talksSqlQueryModel : public QSqlQueryModel {
public:
    ki_talksSqlQueryModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const;
};

class ki_protocolSqlQueryModel : public QSqlQueryModel {
public:
    ki_protocolSqlQueryModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const;
};

class ki_acts_vs2point_SqlQueryModel : public QSqlQueryModel {
public:
    ki_acts_vs2point_SqlQueryModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const;
};

class ki_acts_pol2smo_SqlQueryModel : public QSqlQueryModel {
public:
    ki_acts_pol2smo_SqlQueryModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const;
};

class ki_acts_pol2point_SqlQueryModel : public QSqlQueryModel {
public:
    ki_acts_pol2point_SqlQueryModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const;
};

class ki_packs_SqlQueryModel : public QSqlQueryModel {
public:
    ki_packs_SqlQueryModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const;
};

class ki_stops_SqlQueryModel : public QSqlQueryModel {
public:
    ki_stops_SqlQueryModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const;
};

class ki_hist_SqlQueryModel : public QSqlQueryModel {
public:
    mutable QList<QString> values_up;
    mutable QList<QString> values;

    ki_hist_SqlQueryModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const;
};

#endif // KI_SQLQUERYMODEL

