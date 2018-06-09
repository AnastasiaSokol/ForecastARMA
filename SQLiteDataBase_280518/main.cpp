#include "widget.h"
#include <QApplication>

/*!
* Заголовочный файл cstdio обеспечивает выполнение операций ввода/вывода.
* Операции ввода/вывода в С++ могут быть выполнены, с использованием Стандартной библиотеки ввода/вывода (cstdio в С++, и stdio.h в Cи).
* Эта библиотека использует так называемые потоки для работы с физическими устройствами, такими как клавиатуры, принтеры, терминалы или
* с любыми другими типами файлов, поддерживаемых системой.
*/


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    /*! подключение переводчика-----------------------------------*/
    QTranslator myappTraslator;
    myappTraslator.load(QApplication::applicationDirPath()+
                        QDir::separator()+"lang_ru.qm");
    a.installTranslator(&myappTraslator);

    Widget w;
    w.show();

    return a.exec();
}
