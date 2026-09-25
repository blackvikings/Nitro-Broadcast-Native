#include "nitro/scenes/TransitionEngine.hpp"

namespace nitro {

TransitionEngine::TransitionEngine(QObject* parent)
    : QObject(parent)
{
}

QString TransitionEngine::typeName() const
{
    switch (type_) {
    case TransitionType::Cut: return QStringLiteral("Cut");
    case TransitionType::Fade: return QStringLiteral("Fade");
    }
    return QStringLiteral("None");
}

void TransitionEngine::cut(int fromIndex, int toIndex)
{
    type_ = TransitionType::Cut;
    fromIndex_ = fromIndex;
    toIndex_ = toIndex;
    durationMs_ = 0;
    progress_ = 1.0;
    state_ = TransitionState::Finished;
    emit changed();
    emit completed(toIndex_);
    state_ = TransitionState::Idle;
}

void TransitionEngine::fade(int fromIndex, int toIndex, int durationMs)
{
    type_ = TransitionType::Fade;
    fromIndex_ = fromIndex;
    toIndex_ = toIndex;
    durationMs_ = durationMs > 0 ? durationMs : 300;
    progress_ = 0.0;
    state_ = TransitionState::Running;
    timer_.restart();
    emit changed();
}

void TransitionEngine::tick()
{
    if (state_ != TransitionState::Running) {
        return;
    }
    if (type_ == TransitionType::Cut || durationMs_ <= 0) {
        finish();
        return;
    }
    const double t = static_cast<double>(timer_.elapsed()) / static_cast<double>(durationMs_);
    progress_ = t >= 1.0 ? 1.0 : t;
    emit changed();
    if (progress_ >= 1.0) {
        finish();
    }
}

void TransitionEngine::finish()
{
    progress_ = 1.0;
    state_ = TransitionState::Finished;
    emit changed();
    emit completed(toIndex_);
    state_ = TransitionState::Idle;
    emit changed();
}

} // namespace nitro
