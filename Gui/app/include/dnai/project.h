#ifndef PROJECT_H
#define PROJECT_H

#include "models/entitytree.h"
#include "interfaces/iproject.h"
#include "models/ml/mlproject.h"

namespace dnai {
struct Count {
    int contexts = 0;
    int classes = 0;
    int nodes = 0;
    int functions = 0;
    int variables = 0;
    int inputs = 0;
    int outputs = 0;
    int flows = 0;
};
}

namespace dnai {
    class Project : public models::EntityTree, public interfaces::ASerializable<Project>, public interfaces::IProject
    {
        Q_OBJECT
		Q_PROPERTY(dnai::models::Entity *selectedEntity READ selectedEntity WRITE setSelectedEntity NOTIFY selectedEntityChanged)
		Q_PROPERTY(int childCount READ childCount CONSTANT)
        Q_PROPERTY(bool saved READ saved WRITE setSaved NOTIFY savedChanged)
        Q_PROPERTY(dnai::models::ml::MlProject * mlData READ mlData CONSTANT)

    public:
        Project();
        Project(const QString &filename);
        ~Project();
		const QJsonObject &jsonData() const override;

    // ISerializable implementation
		void serialize(QJsonObject& obj) const override;
		void _deserialize(const QJsonObject& obj) override;
	public:
		models::Entity *selectedEntity() const;
		void setSelectedEntity(models::Entity *entity);
        void setSaved(bool);

	signals:
		void selectedEntityChanged(models::Entity *entity);
        void entityAdded(models::Entity *entity);
        void entityRemoved(models::Entity *entity);
        void savedChanged(bool saveState);

    public:
		Q_INVOKABLE void save() override;
        bool saved() const;
		void load(const QString& path) override;
        void loadFromJson(const QJsonObject& obj);
		void close() override;
		const QString& version() const override;
		void setVersion(const QString& version) override;
		const QString& name() const override;
		void setName(const QString& name) override;
		const QString& description() const override;
		void setDescription(const QString& name) override;
		const QString& fileName() const override;
		void setFileName(const QString& name) override;
		void foreachEntity(const std::function<void(models::Entity*)> &func) const;
	    int childCount() const;
        models::Entity &getRoot() const;
        models::ml::MlProject *mlData();

    public:
        models::ml::MlProject &MlData();

    public:
        Q_INVOKABLE int expandedRows(const QModelIndex& parent) const;
        Q_INVOKABLE int expandedRows() const;

    private:
        QModelIndex getIndexOf(models::Entity *e) const;
        models::Column *getColumnOf(models::Entity *e);

    public slots:
        void removeEntity(dnai::models::Entity *entity);
        void addEntity(dnai::models::Entity *entity);

    public:
        Q_INVOKABLE void addEntityColumnUid(quint32 parentId, QString const &name, QString const &listIndex);
        Q_INVOKABLE QString generateUniqueChildName(dnai::models::Entity *parent) const;

    private:
		template<class T>
        T _foreachEntity(models::Entity* root, const std::function<T(models::Entity*)> &func) const;

    private:
	    QFile* m_file;
	    Count count;
	    models::Entity* m_selectedEntity{};
	    QString m_filename;
	    QString m_description;
	    QJsonObject m_data;
	    QString m_version;
	    models::Entity* m_rootEntity;
        QMap<QString, QString>  m_entityColumnUid;
        models::ml::MlProject m_mlData;
        bool m_save = true;
    };

	//  class Project: public models::Common
    //  {
    //  public:
    //   Project(qint32 uid, QString const& name, QString const& description, QFile& file);
    //      Project(const QString &, const QString &, QFile &);
    //      virtual ~Project();

    //      typedef bool (* searchFunc)(models::Common *, QString const &);
    //      typedef bool (* searchFunctionsFunc)(models::Function *, QString const &);

    //  public:
    //      void save();

    //  private:

    //      QVector<models::Common*> m_index;
    //      QVector<models::Function*> m_functions_index;

    //      models::Context const *m_main;

    //      Count count;

    //  public:
    //      QFile &file() const;
    //      const models::Context *main() const;
    //      QJsonObject data() const;
    //static QJsonObject loadProjectData(const QString &);


    //  private:
    //      models::Variable *unserializeVariable(const QJsonObject &, QObject *);
    //      models::Input *unserializeInput(const QJsonObject &, QObject *);
    //      models::Output *unserializeOutput(const QJsonObject &, QObject *);
    //      models::Flow *unserializeFlow(const QJsonObject &, QObject *);
    //      models::Function *unserializeFunction(const QJsonObject &, QObject *);
    //      models::Class *unserializeClass(const QJsonObject &, QObject *);
    //      models::Context *unserializeContext(const QJsonObject &, QObject *);
    //      models::Node *unserializeNode(const QJsonObject &, QObject *);
    //      static inline QVector2D unserializePosition(const QJsonObject &);


    //      // IModel interface
    //  public:
    //      virtual void serialize(QJsonObject &) const;
    //      virtual void declare() const;

    //  public:
    //      void unserialize(const QJsonObject &);
    //      QVector<models::Common*> search(QString const &, searchFunc = defaultSearchFunc);
    //      QVector<models::Function*> searchFunctions(QString const &, searchFunctionsFunc = defaultSearchFunctionsFunc);

    //  private:
    //      static bool defaultSearchFunc(models::Common *, QString const &);
    //      static bool defaultSearchFunctionsFunc(models::Function *, QString const &);
    //  };
    }

#endif // PROJECT_H
