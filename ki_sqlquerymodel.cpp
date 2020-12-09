#include "ki_sqlquerymodel.h"
#include <QTextCodec>
#include <QDate>
#include <QColor>
#include <QFont>

ki_persSqlQueryModel::ki_persSqlQueryModel(QObject *parent) : QSqlQueryModel(parent)
 { }

QVariant ki_persSqlQueryModel::data(const QModelIndex &index, int role) const {
    QVariant value = QSqlQueryModel::data(index, role);

    QModelIndex index_22 = index.sibling( index.row(),22 );
    QModelIndex index_48 = index.sibling( index.row(),48 );
    QModelIndex index_51 = index.sibling( index.row(),51 );
    QModelIndex index_53 = index.sibling( index.row(),53 );
    QModelIndex index_62 = index.sibling( index.row(),62 );
    QModelIndex index_71 = index.sibling( index.row(),71 );
    QModelIndex index_72 = index.sibling( index.row(),72 );

    // выделим ВС и не выданные полисы цветом
    if( role==Qt::ForegroundRole ) {
        int pol_v = QSqlQueryModel::data(index_53).toInt();
        bool no_get2hand = QSqlQueryModel::data(index_62).isNull();
        if (pol_v==3 && no_get2hand)
            return QColor( 230,0,0 /*Qt::red*/ );
        else if (pol_v==2)
            return QColor( Qt::blue );
        else
            return QColor( Qt::black );
    }

    // выделим не выданные полисы курсивом
    if( role==Qt::FontRole ) {
        int pol_v = QSqlQueryModel::data(index_53).toInt();
        bool no_get2hand = QSqlQueryModel::data(index_62).isNull();
        //QFont font = QFont("Tahoma", 8, -1, true);
        if (pol_v==3 && no_get2hand)
            return QFont("Tahoma", 8, -1, true);
        else if (pol_v==2)
            return QFont("Tahoma", 8, -1, false);
        else
            return QFont("Tahoma", 8, -1, false);
    }

    switch (role) {
        case Qt::DisplayRole: // Данные для отображения
        if (index.column()==6) {
            if (value.toInt()==1) {
                return QString (" М");
            } else if (value.toInt()==2) {
                return QString ("  Ж");
            } else {
                return QString (" ?");
            }
        }
        if (index.column()==7) {
            if (value.toDate()==QDate(1900,1,1)) {
                return QString (" ");
            } else {
                return value.toDate();
            }
        }
        /*if (index.column()==7) {
            if (value.toDate()==QDate(1900,1,1)) {
                return QString (" ");
            } else {
                return value.toDate();
            }
        }*/
        if (index.column()==11) {
            if (value.toInt()==0) {
                return QString (" ");
            } else if (value.toInt()==1) {
                return QString (" бомж");
            } else {
                return QString (" ?");
            }
        }
        if (index.column()==22) {
            if (value.toInt()==0) {
                return QString ("~ новый");
            } else if (value.toInt()==1) {
                return QString ("+ ВС");
            } else if (value.toInt()==2) {
                return QString ("+ полис");
            } else if (value.toInt()==3) {
                return QString ("+ перестр.");
            } else if (value.toInt()==4) {
                return QString ("+ без пол.");
            } else if (value.toInt()==-1) {
                return QString ("- нет ");
            } else if (value.toInt()==-2) {
                return QString ("- др.СМО");
            } else if (value.toInt()==-3) {
                return QString ("- др.терр.");
            } else if (value.toInt()==-4) {
                return QString ("- закрыт ТФ");
            } else if (value.toInt()==-5) {
                return QString ("- отказ ФФ");
            } else if (value.toInt()==-6) {
                return QString ("- дубль");
            } else if (value.toInt()==-10) {
                return QString ("- умер");

            } else if (value.toInt()==11) {
                return QString ("!УДЛ! ВС");
            } else if (value.toInt()==12) {
                return QString ("!УДЛ! полис");
            } else if (value.toInt()==13) {
                return QString ("!УДЛ! перестр.");
            } else if (value.toInt()==14) {
                return QString ("!УДЛ! без пол.");

            } else if (value.toInt()==-666) {
                return QString ("- УДАЛЁН -");
            } else if (value.toInt()>=500) {
                return QString ("в архив (" + QString::number(value.toInt()) + ") ");
            } else if (value.toInt()<=-500) {
                return QString ("! АРХИВ (" + QString::number(value.toInt()) + ") ");

            } else {
                return QString (" ? (" + QString::number(value.toInt()) + ") ");
            }
        }
        if (index.column()==26) {
            if (value.toDate()==QDate(1900,1,1)) {
                return QString (" ");
            } else {
                return value.toDate();
            }
        }
        if (index.column()==49) {
            switch (value.toInt()) {
            case 1:
                return QString ("по терр.");
                break;
            case 2:
                return QString ("заявлен.");
                break;
            default:
                return QString (" - ");
                break;
            }
        }
        if ( index.column()==51 ) {
            switch (value.toInt()) {
            case 0:
                return QString (" ");
                break;
            case 1:
                return QString (" И ");
                break;
            case 2:
                return QString (" Р ");
                break;
            /*default:
                return QString (" - ");
                break;*/
            }
        }
        if (index.column()==70) {
            switch (value.toInt()) {
            case 1:
                return QString ("+ сам придёт");
                break;
            case 2:
                return QString ("!! ПО ПОЧТЕ");
                break;
            default:
                return QString (" - не указал");
                break;
            }
        }

        // 1 then '' when 2 then '' when 3 then '' when 4 then '' when 5 then '' end as talk_type
        if (index.column()==73) {
            switch (value.toInt()) {
            case 1:
                return QString ("СМС (план.)");
                break;
            case 2:
                return QString ("СМС (не пл.)");
                break;
            case 3:
                return QString ("звонок");
                break;
            case 4:
                return QString ("визит");
                break;
            case 5:
                return QString ("звонок в СМО");
                break;
            default:
                return QString (" ? ");
                break;
            }
        }

        // talks.result when -3 then '' when -2 then '' when -1 then '' when 1 then '' when 2 then '' end
        if (index.column()==75) {
            switch (value.toInt()) {
            case -3:
                return QString ("номер/адрес не существует");
                break;
            case -2:
                return QString ("чужой номер/адрес");
                break;
            case -1:
                return QString ("недозвон/нет ответа");
                break;
            case 1:
                return QString ("OK - ответил лично");
                break;
            case 2:
                return QString ("OK - роственик/знакомый");
                break;
            default:
                return QString (" ? ");
                break;
            }
        }
    }

    // выделим ВС с заявкой фоном
    if( role==Qt::BackgroundRole ) {
        bool has_sst = !QSqlQueryModel::data(index_48).toString().trimmed().isEmpty();
        int assig_status = QSqlQueryModel::data(index_51).toInt();
        int status = QSqlQueryModel::data(index_22).toInt();
        int pol_v = QSqlQueryModel::data(index_53).toInt();
        bool has_tfoms = !QSqlQueryModel::data(index_71).isNull();
        bool has_sms = !QSqlQueryModel::data(index_72).isNull();

        if (assig_status<0)                     // ошибка прикрепления
            return QColor( 250, 60, 45 );
        else if (status==-666)                  // удалённая запись
            return QColor( 200,150,150 );
        else if ((status<=0 || status>=500)     // нет полиса
                 /*&& !(status<=-10 && status>=-20)*/)
            return QColor( 222,222,222 );
        else if (status>10 && status<20)        // истёк УДЛ
            return QColor( 245,245,240 );
        else if (!has_sst)                      // не прикреплён к врачу
            return QColor( 255,255,240 );
        else if (has_tfoms)                     // подтверждён ТФОМС
            return QColor( 210,255,230 );
        else
            return QColor( Qt::white );
    }

    return value;
}


ki_polSqlQueryModel::ki_polSqlQueryModel(QObject *parent) : QSqlQueryModel(parent) {
}

QVariant ki_polSqlQueryModel::data(const QModelIndex &index, int role) const {
    QVariant value = QSqlQueryModel::data(index, role);

    /*QModelIndex index_00 = index.sibling( index.row(), 0 );
    QModelIndex index_01 = index.sibling( index.row(), 1 );
    QModelIndex index_02 = index.sibling( index.row(), 2 );
    QModelIndex index_03 = index.sibling( index.row(), 3 );
    QModelIndex index_04 = index.sibling( index.row(), 4 );
    QModelIndex index_05 = index.sibling( index.row(), 5 );*/
    QModelIndex index_06 = index.sibling( index.row(), 6 );
    /*QModelIndex index_07 = index.sibling( index.row(), 7 );
    QModelIndex index_08 = index.sibling( index.row(), 8 );
    QModelIndex index_09 = index.sibling( index.row(), 9 );
    QModelIndex index_10 = index.sibling( index.row(),10 );
    QModelIndex index_11 = index.sibling( index.row(),11 );
    QModelIndex index_12 = index.sibling( index.row(),12 );
    QModelIndex index_13 = index.sibling( index.row(),13 );*/
    QModelIndex index_14 = index.sibling( index.row(),14 );
    /*QModelIndex index_15 = index.sibling( index.row(),15 );
    QModelIndex index_16 = index.sibling( index.row(),16 );
    QModelIndex index_17 = index.sibling( index.row(),17 );
    QModelIndex index_18 = index.sibling( index.row(),18 );
    QModelIndex index_19 = index.sibling( index.row(),19 );
    QModelIndex index_20 = index.sibling( index.row(),20 );
    QModelIndex index_21 = index.sibling( index.row(),21 );
    QModelIndex index_22 = index.sibling( index.row(),22 );
    QModelIndex index_23 = index.sibling( index.row(),23 );
    QModelIndex index_24 = index.sibling( index.row(),24 );
    QModelIndex index_25 = index.sibling( index.row(),25 );
    QModelIndex index_26 = index.sibling( index.row(),26 );
    QModelIndex index_27 = index.sibling( index.row(),27 );
    QModelIndex index_28 = index.sibling( index.row(),28 );
    QModelIndex index_29 = index.sibling( index.row(),29 );
    QModelIndex index_30 = index.sibling( index.row(),30 );
    QModelIndex index_31 = index.sibling( index.row(),31 );
    QModelIndex index_32 = index.sibling( index.row(),32 );
    QModelIndex index_33 = index.sibling( index.row(),33 );
    QModelIndex index_34 = index.sibling( index.row(),34 );*/
    QModelIndex index_35 = index.sibling( index.row(),35 );
    QModelIndex index_36 = index.sibling( index.row(),36 );
    QModelIndex index_37 = index.sibling( index.row(),37 );
    QModelIndex index_38 = index.sibling( index.row(),38 );

    // выделим ВС с СМС, с заявкой фоном
    if( role==Qt::BackgroundRole ) {
        int pol_v = QSqlQueryModel::data(index_06).toInt();
        bool has_tfoms = !QSqlQueryModel::data(index_35).isNull();
        bool has_sms = !QSqlQueryModel::data(index_36).isNull();

        if (pol_v==2 && has_tfoms)
            return QColor( 210,255,230 );
        else
            return QColor( Qt::white );
    }

    // выделим ВС и не выданные полисы цветом
    if( role==Qt::ForegroundRole ) {
        int pol_v = QSqlQueryModel::data(index_06).toInt();
        bool no_get2hand = QSqlQueryModel::data(index_38).isNull();
        bool f_expired = QSqlQueryModel::data(index_14).toDate()<QDate::currentDate();

        if (pol_v==3 && no_get2hand)
            return QColor( 230,0,0 /*Qt::red*/ );
        else if (pol_v==2 && f_expired)
            return QColor( 140,140,140 /*Qt::gray*/ );
        else if (pol_v==2)
            return QColor( Qt::blue );
        else
            return QColor( Qt::black );
    }

    // выделим не выданные полисы курсивом
    if( role==Qt::FontRole ) {
        int pol_v = QSqlQueryModel::data(index_06).toInt();
        bool no_get2hand = QSqlQueryModel::data(index_38).isNull();
        bool f_expired = QSqlQueryModel::data(index_14).toDate()<QDate::currentDate();

        //QFont font = QFont("Tahoma", 8, -1, true);
        if (pol_v==3 && no_get2hand)
            return QFont("Tahoma", 8, -1, true);
        else if (pol_v==2 && f_expired)
            return QFont("Tahoma", 8, -1, true);
        else if (pol_v==2)
            return QFont("Tahoma", 8, -1, false);
        else
            return QFont("Tahoma", 8, -1, false);
    }

    switch (role) {
        case Qt::DisplayRole: // Данные для отображения
        if (index.column()==1) {
            if (value.isNull()) {
                return QString ("");
            } else if (value.toInt()==-1) {
                return QString ("X(") + QString::number(value.toInt()) + ")";
            } else if (value.toInt()==0) {
                return QString ("(") + QString::number(value.toInt()) + ")";
            } else if (value.toInt()==1) {
                return QString ("+(") + QString::number(value.toInt()) + ")";
            } else if (value.toInt()==4) {
                return QString ("+(") + QString::number(value.toInt()) + ")";
            } else {
                return QString ("-(") + QString::number(value.toInt()) + ")";
            }
        }
        if (index.column()==2) {
            if (value.isNull()) {
                return QString ("");
            } else if (value.toInt()==-1) {
                return QString ("X(") + QString::number(value.toInt()) + ")";
            } else if (value.toInt()==0) {
                return QString ("(") + QString::number(value.toInt()) + ")";
            } else if (value.toInt()==1) {
                return QString ("+(") + QString::number(value.toInt()) + ")";
            } else if (value.toInt()==4) {
                return QString ("+(") + QString::number(value.toInt()) + ")";
            } else {
                return QString ("-(") + QString::number(value.toInt()) + ")";
            }
        }
        if ( index.column()==13 ||
             index.column()==14 ||
             index.column()==15 ||
             index.column()==18 ||
             index.column()==35 ||
             index.column()==36 ||
             index.column()==37 ||
             index.column()==38 ) {
            if (value.isNull()) {
                return QString (" ");
            } else if (value.toDate()<QDate::currentDate()) {
                return value.toDate().toString("dd.MM.yyyy");
            } else if (value.toDate()>QDate::currentDate()) {
                return QString("+") + value.toDate().toString("dd.MM.yyyy");
            } else {
                return QString("сегодня");
            }
        }
        if (index.column()==19) {
            if (value.isNull()) {
                return QString ("");
            } else if (value.toInt()==0) {
                return QString ("нет");
            } else if (value.toInt()==1) {
                return QString ("бумажн.");
            } else if (value.toInt()==2) {
                return QString ("карта");
            } else if (value.toInt()==3) {
                return QString ("УЭК");
            } else {
                return QString (" ? ");
            }
        }
    }

    return value;
}


ki_calendarSqlQueryModel::ki_calendarSqlQueryModel(QObject *parent) : QSqlQueryModel(parent)
 { }

QVariant ki_calendarSqlQueryModel::data(const QModelIndex &index, int role) const {
    QVariant value = QSqlQueryModel::data(index, role);

    switch (role) {
        case Qt::DisplayRole: // Данные для отображения
        if (index.column()==2) {
            if (value.toInt()==0) {
                return QString ("- выходной");
            } else if (value.toInt()==1) {
                return QString (" + рабочий");
            } else {
                return QString (" - ? - ");
            }
        }
    }
    return value;
}


ki_names2sexSqlQueryModel::ki_names2sexSqlQueryModel(QObject *parent) : QSqlQueryModel(parent)
 { }

QVariant ki_names2sexSqlQueryModel::data(const QModelIndex &index, int role) const {
    QVariant value = QSqlQueryModel::data(index, role);

    switch (role) {
        case Qt::DisplayRole: // Данные для отображения
        if (index.column()==2) {
            if (value.toInt()==0) {
                return QString (" - ? - ");
            } else if (value.toInt()==1) {
                return QString ("М");
            } else {
                return QString (" Ж");
            }
        }
    }
    return value;
}


ki_docsSqlQueryModel::ki_docsSqlQueryModel(QObject *parent) : QSqlQueryModel(parent)
 { }

QVariant ki_docsSqlQueryModel::data(const QModelIndex &index, int role) const {
    QVariant value = QSqlQueryModel::data(index, role);

    switch (role) {
        case Qt::DisplayRole: // Данные для отображения
        if (index.column()==8) {
            if (value.toInt()==1) {
                return QString (" + ");
            } else if (value.toInt()==2) {
                return QString ("ст.");
            } else {
                return QString (" ");
            }
        }
    }
    return value;
}


ki_talksSqlQueryModel::ki_talksSqlQueryModel(QObject *parent) : QSqlQueryModel(parent)
 { }

QVariant ki_talksSqlQueryModel::data(const QModelIndex &index, int role) const {
    QVariant value = QSqlQueryModel::data(index, role);

    switch (role) {
        case Qt::DisplayRole: // Данные для отображения
        if (index.column()==3) {
            if (QSqlQueryModel::data(index, role).isNull()) {
                return QString (" - ? - ");
            } else if (value.toInt()==-1) {
                return QString (" - ? - ");
            } else if (value.toInt()==0) {
                return QString ("(-) бланк");
            } else if (value.toInt()==1) {
                return QString ("старый");
            } else if (value.toInt()==2) {
                return QString ("   ВС ");
            } else if (value.toInt()==3) {
                return QString ("  ЕНП ");
            }
        }
        if (index.column()==6) {
            if (value.toInt()==1) {
                return QString ("плановое СМС-оповещение о получении полиса");
            } else if (value.toInt()==2) {
                return QString ("неплановое СМС-оповещение");
            } else if (value.toInt()==3) {
                return QString ("звонок из СМО застрахованному");
            } else if (value.toInt()==4) {
                return QString ("визит работника СМО к застрахованному");
            } else if (value.toInt()==5) {
                return QString ("звонок застрахованного в СМО");
            } else if (value.toInt()==6) {
                return QString ("письмо к застрахованному");
            }
        }
        if (index.column()==8) {
            if (value.toInt()==-3) {
                return QString ("(-)  номер / адрес не существует");
            } else if (value.toInt()==-2) {
                return QString ("(-)  чужой номер / адрес");
            } else if (value.toInt()==-1) {
                return QString ("(-)  недозвон / нет ответа");
            } else if (value.toInt()==1) {
                return QString ("(+)  ответил лично");
            } else if (value.toInt()==2) {
                return QString ("(+)  ответил родственик / знакомый");
            } else if (value.toInt()==3) {
                return QString ("(+)  письмо доставлено");
            }
        }
    }
    return value;
}


ki_protocolSqlQueryModel::ki_protocolSqlQueryModel(QObject *parent) : QSqlQueryModel(parent)
 { }

QVariant ki_protocolSqlQueryModel::data(const QModelIndex &index, int role) const {
    QVariant value = QSqlQueryModel::data(index, role);

    QModelIndex index_15 = index.sibling( index.row(),15 );

    // выделим ВС с СМС, с заявкой фоном
    if( role==Qt::BackgroundRole ) {
        int status = QSqlQueryModel::data(index_15).toInt();

        switch (status) {
        case 0:
            return QColor( 255, 50, 50 );   // red
            break;
        case 1:
            return QColor( 255,180, 10 );   // orange
            break;
        case 2:
            return QColor( 255,255,255 );   // white
            break;
        case 3:
            return QColor( 100,255,100 );   // green
            break;
        case -1:
            return QColor( 255,255,100 );   // yellow
            break;
        case -2:
            return QColor( 210,210,210 );   // gray
            break;
        }
    }
    return value;
}


ki_acts_vs2point_SqlQueryModel::ki_acts_vs2point_SqlQueryModel(QObject *parent) : QSqlQueryModel(parent)
 { }

QVariant ki_acts_vs2point_SqlQueryModel::data(const QModelIndex &index, int role) const {
    QVariant value = QSqlQueryModel::data(index, role);

    switch (role) {
        case Qt::DisplayRole: { // Данные для отображения
            if (index.column()==5) {
                if (QSqlQueryModel::data(index, role).isNull()) {
                    return QString (" - головной офис - ");
                }
            }
            if (index.column()==10) {
                if (QSqlQueryModel::data(index, role).toString()=="0 / 0") {
                    return QString ("  ");
                } else {
                    return QSqlQueryModel::data(index, role).toString();
                }
            }
            if (index.column()==11) {
                if (QSqlQueryModel::data(index, role).toInt()>0) {
                    return QString ("исполнен");
                } else {
                    return QString (" -- ");
                }
            }
        }
    }

    // покрасим цветом по статусу актов
    QModelIndex index_0 = index.sibling( index.row(),0 );
    bool f0 = (QSqlQueryModel::data(index_0).toInt()>=0);
    QModelIndex index_11 = index.sibling( index.row(),11 );
    bool ff = (QSqlQueryModel::data(index_11).toInt()>0);

    if( role==Qt::BackgroundRole ) {
        if (f0) {
            if (ff)
                return QColor( 100,255,100 );   // green
            else
                return QColor( 255,255,100 );   // yellow
        } else {
            return QColor( 255,180, 10 );   // orange
        }
    }
    return value;
}


ki_acts_pol2smo_SqlQueryModel::ki_acts_pol2smo_SqlQueryModel(QObject *parent) : QSqlQueryModel(parent)
 { }

QVariant ki_acts_pol2smo_SqlQueryModel::data(const QModelIndex &index, int role) const {
    QVariant value = QSqlQueryModel::data(index, role);

    switch (role) {
        case Qt::DisplayRole: // Данные для отображения
        if (index.column()==5) {
            if (QSqlQueryModel::data(index, role).isNull()) {
                return QString (" - головной офис - ");
            }
        }
    }

    // покрасим цветом по статусу актов
    QModelIndex index_0 = index.sibling( index.row(),0 );
    bool f0 = (QSqlQueryModel::data(index_0).toInt()>=0);
    QModelIndex index_11 = index.sibling( index.row(),11 );
    bool ff = (QSqlQueryModel::data(index_11).toInt()>0);

    if( role==Qt::BackgroundRole ) {
        if (f0) {
        } else {
            return QColor( 255,180, 10 );   // orange
        }
    }
    return value;
}


ki_acts_pol2point_SqlQueryModel::ki_acts_pol2point_SqlQueryModel(QObject *parent) : QSqlQueryModel(parent)
 { }

QVariant ki_acts_pol2point_SqlQueryModel::data(const QModelIndex &index, int role) const {
    QVariant value = QSqlQueryModel::data(index, role);

    switch (role) {
        case Qt::DisplayRole: { // Данные для отображения
            if (index.column()==5) {
                if (QSqlQueryModel::data(index, role).isNull()) {
                    return QString (" - головной офис - ");
                }
            }
            if (index.column()==10) {
                if (QSqlQueryModel::data(index, role).toString()=="0 / 0") {
                    return QString ("  ");
                } else {
                    return QSqlQueryModel::data(index, role).toString();
                }
            }
            if (index.column()==11) {
                if (QSqlQueryModel::data(index, role).toInt()>0) {
                    return QString ("исполнен");
                } else {
                    return QString (" -- ");
                }
            }
        }
    }

    // покрасим цветом по статусу актов
    QModelIndex index_0 = index.sibling( index.row(),0 );
    bool f0 = (QSqlQueryModel::data(index_0).toInt()>=0);
    QModelIndex index_11 = index.sibling( index.row(),11 );
    bool ff = (QSqlQueryModel::data(index_11).toInt()>0);

    if( role==Qt::BackgroundRole ) {
        if (f0) {
            if (ff)
                return QColor( 100,255,100 );   // green
            else
                return QColor( 255,255,100 );   // yellow
        } else {
            return QColor( 255,180, 10 );   // orange
        }
    }
    return value;
}


ki_packs_SqlQueryModel::ki_packs_SqlQueryModel(QObject *parent) : QSqlQueryModel(parent)
 { }

QVariant ki_packs_SqlQueryModel::data(const QModelIndex &index, int role) const {
    QVariant value = QSqlQueryModel::data(index, role);

    QModelIndex index_2 = index.sibling( index.row(),2 );
    bool fi = !QSqlQueryModel::data(index_2).isNull();
    QModelIndex index_6 = index.sibling( index.row(),6 );
    bool fp = !QSqlQueryModel::data(index_6).isNull();
    QModelIndex index_14= index.sibling( index.row(),14);
    bool ff = !QSqlQueryModel::data(index_14).isNull();

    // выделим ВС с СМС, с заявкой фоном
    if( role==Qt::BackgroundRole ) {

        if (ff)
            return QColor( 255,255,100 );   // yellow
        else
        if (fp)
            return QColor( 100,255,100 );   // green
        else
        if (fi)
            return QColor( 255,100,100 );   // red
        else
            return QColor( 210,210,210 );   // gray
    }
    return value;
}


ki_stops_SqlQueryModel::ki_stops_SqlQueryModel(QObject *parent) : QSqlQueryModel(parent)
 { }

QVariant ki_stops_SqlQueryModel::data(const QModelIndex &index, int role) const {
    QVariant value = QSqlQueryModel::data(index, role);

    /*QModelIndex index_2 = index.sibling( index.row(),2 );
    bool fi = !QSqlQueryModel::data(index_2).isNull();
    QModelIndex index_6 = index.sibling( index.row(),6 );
    bool fp = !QSqlQueryModel::data(index_6).isNull();
    QModelIndex index_14= index.sibling( index.row(),14);
    bool ff = !QSqlQueryModel::data(index_14).isNull();

    // выделим ВС с СМС, с заявкой фоном*/
    if( role==Qt::BackgroundRole )

       // if (ff)*/
            return QColor( 255,200,100 );   // yellow
       /* else
        if (fp)
            return QColor( 100,255,100 );   // green
        else
        if (fi)
            return QColor( 255,100,100 );   // red
        else
            return QColor( 210,210,210 );   // gray
    }*/
    return value;
}


ki_hist_SqlQueryModel::ki_hist_SqlQueryModel(QObject *parent) : QSqlQueryModel(parent)
 { }

QVariant ki_hist_SqlQueryModel::data(const QModelIndex &index, int role) const {
    QVariant value = QSqlQueryModel::data(index, role);
    int row_n = index.row();

    if ( role==Qt::DisplayRole ) { // Данные для отображения
        // если надо - дополним список старых и новых значений
        QString s_null = "";
        int col_num = index.column();

        while (values_up.count()<=col_num)
            values_up.append("");
        while (values.count()<=col_num)
            values.append(s_null);

        // обновим значение
        values_up[col_num] = QString(values.at(col_num));
        values[col_num] = value.toString();
    }

    // выделим заявкой фоном изменённые значения
    if( role==Qt::BackgroundRole ) {

        if ( row_n==0
             || values_up.at(index.column())!=values.at(index.column()))
            return QColor( 255,180, 10 );   // orange

        return QColor( 254,254,254 );   // white
    }
    return value;
}
