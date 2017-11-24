#include "dulysettings.h"
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QQuickItem>

namespace duly_gui
{
	DulySettings::DulySettings(QObject* parent) : QObject(parent)
	{
		m_style = new DulySettingsModel(nullptr);
		m_settings.clear();
		const auto theme = m_settings.value("themes/current/init").toString();
		m_isInit = theme != "";
	}

	DulySettings::~DulySettings()
	{
	}

	void DulySettings::setStyle(DulySettingsModel* m)
	{
		if (m == m_style)
			return;
		m_style = m;
		emit styleChanged(m);
	}

	void DulySettings::loadTheme(const QString&path)
	{
		m_loadedColors.clear();
		m_loadedNumbers.clear();
		QFile file(m_themesPath[path]);

		if (!file.open(QIODevice::ReadOnly)) {
			return;
		}

		const auto data = file.readAll();
		const auto obj(QJsonDocument::fromJson(data).object());
		const auto pair = findObject(obj, "");
		const auto loadThemeName = "themes/" + path;
		const auto currentThemeName = "themes/current";
		for (auto i = 0; i < pair.first.count(); i++)
		{
			const auto key = pair.first[i];
            const auto value = pair.second[i];
            m_settings.setValue(currentThemeName + key, value);
			m_settings.setValue(loadThemeName + key, value);
//			qDebug() << currentThemeName + key;
//			qDebug() << value;
			if (m_style)
			{
                updateProperty(key.mid(1), value);
			}
		}
		m_settings.setValue("themes/current/init", "0");
	}

	void DulySettings::init()
    {
		const auto theme = m_settings.value("themes/current/init").toString();
		QDir dir("settings/themes");
		const auto list = dir.entryList(QDir::Files);
		for (auto i = list.begin(); i!= list.end(); ++i)
		{
			const auto f = QFileInfo(*i);
			const auto basename = f.baseName();
			m_themes.append(basename);
			m_themesPath[basename] = dir.absoluteFilePath(*i);
			
		}
		m_isInit = theme != "";
		if (!m_isInit && m_themes.count() != 0)
        {
			loadTheme(m_themes[0]);
		}
	}

	QStringList DulySettings::getThemes() const
	{
		return  m_themes;
	}

	bool DulySettings::isSettingsLoad() const
	{
		return m_isInit;
    }

	void DulySettings::updateProperty(const QString &path, const QVariant &variant)
	{
		const auto pair = getFinalProperty(m_style, path);
        QFontDatabase database;
		QString family = "";

		if (pair.second == "family")
		{
			for (auto i = 0; i < m_families.count(); i++)
			{
                if (database.hasFamily(m_families[i]))
                    family = m_families[i];
			}
            if (family == "")
            {
                const auto id = QFontDatabase::addApplicationFont(variant.toString());
                family = QFontDatabase::applicationFontFamilies(id).at(0);
                m_families.append(family);
            }
			pair.first->setProperty(pair.second.toLatin1().data(), family);
		}
		else
		{
			//		qDebug() << pair.first << pair.second;
			pair.first->setProperty(pair.second.toLatin1().data(), variant);
		}
	}

	void DulySettings::registerStyle(DulySettingsModel *style)
	{
		m_style = style;
	}

	QPair<QQuickItem*, QString> DulySettings::getFinalProperty(QQuickItem *item, const QString &path) const
	{
		QPair<QQuickItem*, QString> pair;
        auto list = path.split("/");
        QByteArray *qb = new QByteArray(list[0].toLatin1());
        if (list.count() > 1)
        {
            const auto newPath = path.section('/', 1, -1);
/*            qDebug() << "New path (" + newPath + ")";
            qDebug() << "attr (" << list[0] << "," << qb->constData() << ")";
            qDebug() << "<--" << item;*/
            const auto prop = item->property(qb->constData());
            item = qvariant_cast<QQuickItem *>(prop);
            //qDebug() << "-->" << item;
			pair = getFinalProperty(item, newPath);
        }
        else
        {
            pair.first = item;
			pair.second = list[0];
            return pair;
        }
		return pair;
	}

	qreal DulySettings::getSettingNumber(const QString &path)
	{
		
		if (m_loadedNumbers.contains(path))
		{

			return m_loadedNumbers[path];
		}
		const auto s = m_settings.value(path).toReal();
		m_loadedNumbers[path] = s;
		return m_loadedNumbers[path];
	}

	QPair<QStringList, QList<QVariant>> DulySettings::findObject(QJsonObject obj, const QString root)
	{
		QPair<QStringList, QList<QVariant>> pair;
		for (auto it = obj.begin(); it != obj.end(); ++it) {
			const auto key = it.key();
			const auto value = it.value();

			if (value.isObject())
			{
				const auto p = findObject(value.toObject(), root + "/" + key);
				pair.first.append(p.first);
				pair.second.append(p.second);
			}
			else
			{
				pair.first.append(root + "/" + key);
				pair.second.append(value.toVariant());
			}
		}
		return pair;
	}
}
