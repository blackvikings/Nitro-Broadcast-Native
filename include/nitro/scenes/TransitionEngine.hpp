#pragma once

#include <QObject>
#include <QElapsedTimer>
#include <atomic>

namespace nitro {

enum class TransitionType {
    Cut,
    Fade
};

enum class TransitionState {
    Idle,
    Running,
    Finished
};

class TransitionEngine : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool active READ isActive NOTIFY changed)
    Q_PROPERTY(double progress READ progress NOTIFY changed)
    Q_PROPERTY(QString typeName READ typeName NOTIFY changed)
    Q_PROPERTY(int fromIndex READ fromIndex NOTIFY changed)
    Q_PROPERTY(int toIndex READ toIndex NOTIFY changed)

public:
    explicit TransitionEngine(QObject* parent = nullptr);

    Q_INVOKABLE void cut(int fromIndex, int toIndex);
    Q_INVOKABLE void fade(int fromIndex, int toIndex, int durationMs = 300);
    Q_INVOKABLE void tick(); // call from UI/timer (~60 Hz)

    bool isActive() const { return state_ == TransitionState::Running; }
    double progress() const { return progress_; }
    TransitionState state() const { return state_; }
    TransitionType type() const { return type_; }
    QString typeName() const;
    int fromIndex() const { return fromIndex_; }
    int toIndex() const { return toIndex_; }
    int durationMs() const { return durationMs_; }

signals:
    void changed();
    void completed(int toIndex);

private:
    void finish();

    TransitionType type_ = TransitionType::Cut;
    TransitionState state_ = TransitionState::Idle;
    int fromIndex_ = -1;
    int toIndex_ = -1;
    int durationMs_ = 0;
    double progress_ = 0.0;
    QElapsedTimer timer_;
};

} // namespace nitro
