#include <QCoreApplication>
#include <QtTest/QtTest>

extern int run_scene_tests(int, char**);
extern int run_settings_tests(int, char**);
extern int run_mixer_tests(int, char**);
extern int run_wasapi_tests(int, char**);

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    int status = 0;
    status |= run_scene_tests(argc, argv);
    status |= run_settings_tests(argc, argv);
    status |= run_mixer_tests(argc, argv);
    status |= run_wasapi_tests(argc, argv);
    return status;
}
