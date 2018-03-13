#include <QJsonDocument>

#include "dnai/solution.h"
#include "dnai/editor.h"
#include "dnai/project.h"
#include "dnai/exceptions/guiexception.h"

namespace dnai
{
	Solution::Solution(): m_selectedProject(nullptr), m_file(nullptr)
	{
	}

	void Solution::save()
	{
		QJsonObject obj;
		serialize(obj);

		m_file->write(QJsonDocument(obj).toJson());
	}

	void Solution::load(const QString& path)
	{
		m_filename = path;
		m_file = new QFile(QUrl(m_filename).toLocalFile());

		if (!m_file->open(QIODevice::ReadWrite)) {
			qWarning("Couldn't open file.");
			return;
		}

		const QByteArray data = m_file->readAll();

		try {
			const QJsonObject obj(QJsonDocument::fromJson(data).object());
			_deserialize(obj);
			m_data = obj;
			m_selectedProject = m_projects[0];
		}
		catch (std::exception &e) {
			Q_UNUSED(e)
			throw exceptions::GuiExeption("Error : Corrupted Solution file");
		}
		qWarning("Couldn't parse file.");
	}

	void Solution::close()
	{
		for (const auto proj : m_projects)
			proj->close();
	}

	const QString& Solution::version() const
	{
		return m_version;
	}

	void Solution::setVersion(const QString& version)
	{
		if (m_version == version)
			return;
		m_version = version;
	}

	const QString& Solution::name() const
	{
		return m_name;
	}

	void Solution::setName(const QString& name)
	{
		if (name == m_name)
			return;
		m_name = name;
	}

	const QString& Solution::description() const
	{
		return m_description;
	}

	void Solution::setDescription(const QString& desc)
	{
		if (m_description == desc)
			return;
		m_description = desc;
	}

	const QJsonObject& Solution::jsonData() const
	{
		return m_data;
	}

	const QList<interfaces::IProject*>& Solution::projects() const
	{
		return m_projects;
	}

	void Solution::addProject(IProject *proj)
	{
		if (!m_projects.contains(proj))
			m_projects.append(proj);
	}

	interfaces::IProject *Solution::seletedProject() const
	{
		return m_selectedProject;
	}

	bool Solution::selectProject(IProject *proj)
	{
		if (proj == m_selectedProject)
			return false;
		m_selectedProject = proj;
		return true;
	}

	void Solution::serialize(QJsonObject& obj) const
	{
		obj["projects"] = serializeList<IProject>(m_projects);
	}

	const QString& Solution::fileName() const
	{
		return m_filename;
	}

	void Solution::setFileName(const QString& name)
	{
		if (m_filename == name)
			return;
		m_filename = name;
	}

	void Solution::_deserialize(const QJsonObject& obj)
	{
		if (obj["version"].toString() != Editor::instance().version())
			qWarning() << "Warning this solution file (" << m_filename << ") wasn't created with the same editor's version (" << obj["version"].toString() << "!= current" << Editor::instance().version() << ")";
		for (const auto projfilename : obj["projects"].toArray())
		{
			const auto proj = new Project();
			proj->load(projfilename.toString());
			m_projects.append(proj);
		}
	}
}
