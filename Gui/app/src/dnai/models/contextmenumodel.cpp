#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

#include "dnai/models/contextMenuModel.h"
#include "dnai/models/entity.h"
#include "dnai/enums/core/instructionid.h"

namespace dnai
{
	namespace models
	{
		const QString& ContextMenuItem::name() const
		{
			return m_name;
		}

		const QString& ContextMenuItem::description() const
		{
			return m_descrition;
		}

		int ContextMenuItem::inputs() const
		{
			return m_inputs;
		}

		int ContextMenuItem::outputs() const
		{
			return m_outputs;
		}

		int ContextMenuItem::instructionId() const
		{
			return m_instructionId;
		}

		const QString& ContextMenuItem::nodeName() const
		{
			return m_nodeName;
		}

		void ContextMenuItem::setNodeName(const QString& name)
		{
			if (name == m_nodeName)
				return;
			m_nodeName = name;
            emit nodeNameChanged(name);
        }

        void ContextMenuItem::setType(int t)
        {
            if (t == m_type)
                return;
            m_type = t;
            emit typeChanged(t);
        }

        void ContextMenuItem::setConstruction(const QList<qint32> &value)
        {
            if (m_construction == value)
                return;
            m_construction = value;
            emit constructionChanged(m_construction);
        }

		void ContextMenuItem::setName(const QString& name)
		{
			if (name == m_name)
				return;
			m_name = name;
			emit nameChanged(name);
		}

		void ContextMenuItem::setDescription(const QString& descr)
		{
			if (descr == m_descrition)
				return;
			m_descrition = descr;
			emit descriptionChanged(descr);
		}

		void ContextMenuItem::setInputs(int i)
		{
			if (i == m_inputs)
				return;
			m_inputs = i;
			emit inputsChanged(i);
		}

		void ContextMenuItem::setOutputs(int o)
		{
			if (o == m_outputs)
				return;
			m_outputs = o;
			emit outputsChanged(o);
		}

		void ContextMenuItem::setInstructionId(int instr)
		{
			if (instr == m_instructionId)
				return;
			m_instructionId = instr;
			emit instructionIdChanged(instr);
		}

		int ContextMenuItem::columnCount() const
		{
            return 1;
        }

        int ContextMenuItem::type() const
        {
            return m_type;
        }

        const QList<qint32> &ContextMenuItem::construction() const
        {
            return m_construction;
        }

		ContextMenuModel::ContextMenuModel(QObject* parent)
        : QAbstractItemModel(parent), m_root(nullptr), m_variableGetter(nullptr)
		{
		}

		ContextMenuModel::ContextMenuModel(const QJsonObject &doc, QObject* parent)
        : QAbstractItemModel(parent), m_root(nullptr), m_variableGetter(nullptr)
		{
            /*
             * Create root node
             */
            m_root = new ContextMenuItem();
            m_root->setName("__Root");
            /*
             * Create variableGetter node
             */
            m_variableGetter = new ContextMenuItem();
            m_variableGetter->setName("Variables getter");
            m_root->appendChild(m_variableGetter);
            /*
             * Create variableSetter node
             */
            m_variableSetter = new ContextMenuItem();
            m_variableSetter->setName("Variables setter");
            /*
             * Populate root model with getter and setter nodes
             */
            m_root->appendChild(m_variableGetter);

			parseJsonDocument(doc);
		}

		void ContextMenuModel::parseJsonDocument(const QJsonObject& json)
        {
			parseJsonObj(m_root, json);
		}

		void ContextMenuModel::parseJsonObj(ContextMenuItem *parent, const QJsonObject &js)
		{
			for (const auto& key : js.keys())
			{
				if (key == "categories")
				{
					const auto categories = js[key].toObject();
					for (const auto& categoryKey : categories.keys())
					{
						const auto categoryObj = categories[categoryKey].toObject();
						auto category = new ContextMenuItem();
						category->setName(categoryKey);
						category->setNodeName(categoryObj["name"].toString());
                        category->setDescription(categoryObj["description"].toString());
                        if (categoryObj.contains("construction"))
                        {
                            QList<qint32> ctrs;

                            for (QJsonValue val : categoryObj["construction"].toArray())
                            {
                                ctrs.append(val.toVariant().toInt());
                            }
                            category->setConstruction(ctrs);
                        }
                        if (parent && categoryObj["inputs"].toInt() == 0 && parent->inputs() != 0)
                            category->setInputs(parent->inputs());
                        else
                            category->setInputs(categoryObj["inputs"].toInt());
                        if (parent && categoryObj["outputs"].toInt() == 0 && parent->outputs() != 0)
                            category->setOutputs(parent->outputs());
                        else
                            category->setOutputs(categoryObj["outputs"].toInt());
                        if (parent && categoryObj["type"].toInt() == 0 && parent->type() != -1)
                            category->setType(parent->type());
                        else
                            category->setType(categoryObj["type"].toInt());
                        if (!categoryObj["instruction_id"].isUndefined())
                        {
                            const auto instruction_id = categoryObj["instruction_id"].toInt();
                            m_hash[instruction_id] = category;
                            category->setInstructionId(instruction_id);
                        }
						parent->appendChild(category);
						if (categoryObj.constFind("categories") != categoryObj.constEnd())
							parseJsonObj(category, categoryObj);
					}
				}
			}
		}

		QModelIndex ContextMenuModel::index(int row, int column, const QModelIndex& parent) const
		{
			if (!hasIndex(row, column, parent))
			{
				return {};
			}
			ContextMenuItem *parentItem;

			if (!parent.isValid())
				parentItem = m_root;
			else
				parentItem = static_cast<ContextMenuItem*>(parent.internalPointer());
			const auto childItem = parentItem->child(row);
			if (childItem)
			{
				return createIndex(row, column, childItem);
			}
			return {};
		}

		QModelIndex ContextMenuModel::parent(const QModelIndex& child) const
		{
			if (!child.isValid())
				return {};

			ContextMenuItem *childItem = getItem(child);
			ContextMenuItem *parentItem = childItem->parentItem();

			if (parentItem == m_root)
				return QModelIndex();
			return createIndex(parentItem->row(), 0, parentItem);
		}

		ContextMenuItem *ContextMenuModel::getItem(const QModelIndex &index) const
		{
			if (index.isValid()) {
				ContextMenuItem *item = static_cast<ContextMenuItem*>(index.internalPointer());
				if (item)
					return item;
			}
			return m_root;
		}

		int ContextMenuModel::rowCount(const QModelIndex& parent) const
		{
			ContextMenuItem *parentItem;
			if (parent.column() > 0)
				return 0;

			if (!parent.isValid())
				parentItem = m_root;
			else
				parentItem = static_cast<ContextMenuItem*>(parent.internalPointer());
			return parentItem->childCount();
		}

		int ContextMenuModel::columnCount(const QModelIndex& parent) const
		{
			if (parent.isValid())
				return static_cast<ContextMenuItem*>(parent.internalPointer())->columnCount();
			return m_root->columnCount();
		}

		QVariant ContextMenuModel::data(const QModelIndex& index, int role) const
		{
			if (!index.isValid())
				return QVariant();
			const auto entity = static_cast<ContextMenuItem*>(index.internalPointer());
			switch (role)
			{
			case ITEM:
				return QVariant::fromValue(entity);
			case DESCRIPTION:
				return entity->description();
			case NAME:
			case Qt::DisplayRole:
				return entity->name();
			case INPUTS:
				return entity->inputs();
			case OUTPUTS:
				return entity->outputs();
			case INSTRUCTION_ID:
				return entity->instructionId();
            case CONSTRUCTION:
                return QVariant::fromValue(entity->construction());
			default:
				return QVariant();
            }
        }

        QHash<int, QByteArray> ContextMenuModel::roleNames() const
        {
            QHash<int, QByteArray> hash;
            hash[ITEM] = "item";
            hash[DESCRIPTION] = "description";
            hash[NAME] = "name";
            hash[INPUTS] = "inputs";
            hash[OUTPUTS] = "outputs";
            hash[INSTRUCTION_ID] = "instruction_id";
            hash[CONSTRUCTION] = "construction";
            return hash;
        }

        const QHash<int, ContextMenuItem *> &ContextMenuModel::instructions() const
        {
            return m_hash;
        }

        void ContextMenuModel::appendVariable(Entity *entity)
        {
            /*
             * Create contextItem for getter
             */
            auto item = new ContextMenuItem();
            item->setName(entity->name());
            item->setDescription(entity->description());
            item->setInputs(0);
            item->setOutputs(1);
            item->setType(entity->entityType());
            item->setInstructionId(dnai::enums::QInstructionID::GETTER);
            /*
             * Append variable into getter list
             */
            beginInsertRows(index(0, 0, QModelIndex()), m_variableGetter->childCount(), m_variableGetter->childCount());
            m_variableGetter->appendChild(item);
            endInsertRows();

            /*
             * Create contextItem for setter
             */
            item = new ContextMenuItem();
            item->setName(entity->name());
            item->setDescription(entity->description());
            item->setInputs(1);
            item->setOutputs(0);
            item->setType(entity->entityType());
            item->setInstructionId(dnai::enums::QInstructionID::SETTER);
            /*
             * Append variable into setter list
             */
            beginInsertRows(index(1, 0, QModelIndex()), m_variableSetter->childCount(), m_variableSetter->childCount());
            m_variableSetter->appendChild(item);
            endInsertRows();
        }
	}
}