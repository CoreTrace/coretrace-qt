#ifndef UPDATE_OBSERVER_HPP
#define UPDATE_OBSERVER_HPP

#include <QString>

// Observer interface for update notifications
class UpdateObserver {
public:
    virtual ~UpdateObserver() = default;
    virtual void onUpdateAvailable(const QString& newVersion, const QString& downloadUrl) = 0;
};

#endif // UPDATE_OBSERVER_HPP 