#include <QFileInfo>
#include "dnai/http/response.h"
#include "dnai/api/api.h"
#include "dnai/project.h"
#include "dnai/editor.h"
#include "dnai/solution.h"
#include "dnai/session.h"

namespace dnai {
//namespace http {
Session::Session(QObject *parent)
    : QObject(parent),
    m_user(nullptr),
    m_rememberUser(false)
{
}

models::User *Session::user() const
{
    return m_user;
}

bool Session::rememberUser() const
{
    return m_rememberUser;
}

void Session::setUser(models::User *user)
{
    if (user == m_user)
        return;
    m_user = user;
    emit userChanged(user);
}

void Session::signin(const QString &username, const QString &password)
{
    api::signin(username, password).map([this](http::Response response) -> http::Response {
        getCurrentUser();
        return response;
    },
    [this](http::Response response) -> http::Response {
        emit apiErrors();
        return response;
    });
}

void Session::getCurrentUser()
{
    if (dnai::api::getId().size() == 0) {
        return;
    }
    api::get_current_user().map([this](http::Response response) -> http::Response {
        m_user = new models::User();
        m_user->setName(response.body["username"].toString());
        m_user->setProfile_url("../Images/default_user.png");
        updateCurentUserFiles();
        Editor::instance().notifyInformation("Welcome back " + m_user->name() + " !", []() {});
        emit userChanged(m_user);
        return response;
    });
}

bool Session::uploadFile(const QString &path)
{
    auto file = new QFile(QUrl(path).toLocalFile());
    if (!file->open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open file.");
        return false;
    }
    api::post_file(QFileInfo(file->fileName()).fileName(), file).map([this](http::Response response) -> http::Response {
        updateCurentUserFiles();
        return response;
    });
    return true;
}

void Session::updateCurentUserFiles()
{
    api::get_files().map([this](http::Response response) -> http::Response {
        if (m_user != nullptr) {
            m_user->setFiles(response.body["files"].toArray());
            emit userChanged(m_user);
        }
        return response;
    });
}

void Session::logout()
{
    api::logout();
    delete m_user;
    setUser(nullptr);
}

void Session::downloadProjectData(uint index, const QString &id)
{
    api::get_raw_file(id).map([this, index](http::Response response) -> http::Response {
        auto solution = Editor::instance().solution();
		auto project = new Project();
		if (!solution)
		{
            Editor::instance().loadSolution("");
            solution = Editor::instance().solution();
			solution->setName("Solution " + project->name());
		}
		project->loadFromJson(response.body);
		solution->addProject(project);
		m_user->setCurrentFileData(project->jsonData());
		emit userChanged(m_user);
        return response;
     });
}

void Session::setRememberUser(bool user)
{
    m_rememberUser = user;
    api::rememberUser = user;
    emit rememberUserChanged(m_rememberUser);
}

}
