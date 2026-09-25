#pragma once

#include <QJsonObject>

namespace nitro {

struct SourceCrop {
    double left = 0.0;
    double right = 0.0;
    double top = 0.0;
    double bottom = 0.0;

    QJsonObject toJson() const;
    static SourceCrop fromJson(const QJsonObject& obj);
};

struct SourceTransform {
    double x = 0.0;
    double y = 0.0;
    double width = 1920.0;
    double height = 1080.0;
    double rotation = 0.0;
    double opacity = 1.0;
    SourceCrop crop;

    void reset(double canvasW = 1920.0, double canvasH = 1080.0);
    void fitToCanvas(double canvasW, double canvasH, double contentW, double contentH);
    void centerOnCanvas(double canvasW, double canvasH);
    void stretchToCanvas(double canvasW, double canvasH);

    QJsonObject toJson() const;
    static SourceTransform fromJson(const QJsonObject& obj);
};

} // namespace nitro
