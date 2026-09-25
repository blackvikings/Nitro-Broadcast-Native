#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QtTest/QtTest>

extern int run_scene_tests(int, char**);
extern int run_settings_tests(int, char**);
extern int run_mixer_tests(int, char**);
extern int run_wasapi_tests(int, char**);
extern int run_hardening_tests(int, char**);

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    QFile log(QStringLiteral("nitro_test_summary.txt"));
    log.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QTextStream out(&log);

    auto run = [&](const char* name, int (*fn)(int, char**)) {
        const int code = fn(argc, argv);
        out << name << " => " << code << "\n";
        out.flush();
        return code;
    };

    int status = 0;
    status |= run("scenes", run_scene_tests);
    status |= run("settings", run_settings_tests);
    status |= run("mixer", run_mixer_tests);
    status |= run("wasapi", run_wasapi_tests);
    status |= run("hardening", run_hardening_tests);
    out << "TOTAL => " << status << "\n";
    return status;
}
