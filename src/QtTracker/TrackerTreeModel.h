#pragma once 

#include <QApplication>
#include <QFile>
#include <QTreeView>
#include <QMetaEnum>
#include <vrdelta.h>




// basic idea is that the implementor provides
// * index objects on demand
// * values for indexes on demand
//
// my structure is similar
// SDECLS and VDECLS	--> are leaf rows
// vector<> represent	--> rows with children
// 
// a runtime traversal could build a map between rows and nodes
// or a compile time template instantiation is plausible
//

// proposal is tracker can build an exported tree
// decouples ui from interface
// TrackerNodeIF

class TrackerTreeModel : public QAbstractItemModel
{
	Q_OBJECT
private:
	TrackerNodeIF *root_tracker_node;

public:

	TrackerTreeModel(TrackerNodeIF *root_node)
		: root_tracker_node(root_node)
	{}

	// convert parent_index to a tracker node if it is valid.  otherwise return root
	TrackerNodeIF *trackerNode(const QModelIndex &parent_index) const
	{
		TrackerNodeIF *parent_node;
		if (!parent_index.isValid())
			parent_node = root_tracker_node;
		else
			parent_node = static_cast<TrackerNodeIF*>(parent_index.internalPointer());
		return parent_node;
	}

	bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE
	{
		//If row is rowCount(), the rows are appended to any existing rows in the parent.
		if (row == rowCount(parent) && count > 0)
		{
			beginInsertRows(parent, rowCount(parent), rowCount(parent) + count - 1);
#if 0
			TrackerNodeIF *node = trackerNode(parent);
			for (int i = 0; i < count; i++)
			{
				Stringy *s = (Stringy *)node;
				Stringy *new_child = new Stringy(s, "baby", "godzilla");
				s->children.push_back(new_child);
			}
#endif
			endInsertRows();
			return true;
		}
		else
		{
			return false;
		}

	}


	QVariant headerData(int section, Qt::Orientation orientation,
		int role) const
	{
		QMetaEnum metaEnum = QMetaEnum::fromType<Qt::Orientation>();
		QString o = metaEnum.valueToKey(orientation);


		Qt::ItemDataRole roley = (Qt::ItemDataRole)role;
		QMetaEnum metaEnum2 = QMetaEnum::fromType<Qt::ItemDataRole>();
		QString r = metaEnum2.valueToKey(roley);


		//QMetaEnum::fromType<Qt::Orientation> m();

		QString my_formatted_string = QString("%1 %2 %3").arg(QString::number(section), o, r);

		if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
			return QVariant(my_formatted_string);
		return QVariant();
	}

	QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE
	{
		if (!index.isValid())
			return QVariant();

		if (role != Qt::DisplayRole)
			return QVariant();

		// indexes are trackerNodeIfs
		TrackerNodeIF * tnode = trackerNode(index);

		

		if (tnode->GetParent() && strcmp(tnode->GetParent()->GetLabel(), "system") == 0)
		{
			if (index.row() == 0)
			{
				return QVariant(tnode->GetPopupMenuLabel());
			}
			else
			{
				std::string s = tnode->GetChangeDescriptionString().c_str();
				s.resize(20);
				return QVariant(s.c_str());
			}
		}
		
		if (index.column() == 0)
		{
			return QVariant(tnode->GetPopupMenuLabel());
		}
		else if (index.column() == 1)
		{
			std::string s = tnode->GetChangeDescriptionString().c_str();
			s.resize(20);
			return QVariant(s.c_str());
		}
		else
		{
			return QVariant();
		}
	}

	Qt::ItemFlags flags(const QModelIndex &index) const
	{
		if (!index.isValid())
			return 0;

		return QAbstractItemModel::flags(index);
	}

	QModelIndex index(int row, int column,
		const QModelIndex &parent_index = QModelIndex()) const Q_DECL_OVERRIDE
	{
		if (!hasIndex(row, column, parent_index))
			return QModelIndex();

		TrackerNodeIF *parent_node = trackerNode(parent_index);

		TrackerNodeIF *child_node;
		if (parent_node && strcmp(parent_node->GetLabel(), "system")==0)
		{
			// the column chooses the child
			child_node = parent_node->GetChild(column);		
		}
		else
		{
			child_node = parent_node->GetChild(row);
		}

		
		if (child_node)
			return createIndex(row, column, child_node);
		else
			return QModelIndex();
	}

	QModelIndex parent(const QModelIndex &index) const
	{
		if (!index.isValid())
			return QModelIndex();

		TrackerNodeIF *child = trackerNode(index);
		TrackerNodeIF *parent = child->GetParent();

		if (parent == root_tracker_node)
			return QModelIndex();

		return createIndex(child->GetMyIndexInParent(), 0, parent);
	}
	
	int columnCount(const QModelIndex &parent) const Q_DECL_OVERRIDE
	{
		int num_columns = 100;
#if 0
		TrackerNodeIF *parent_node = trackerNode(parent);
		TrackerNodeIF *parent_parent_node = parent_node->GetParent();

		if (parent_node && strcmp(parent_node->GetLabel(), "system") == 0)
		{
			num_columns = 100;
	//		num_columns = parent_node->GetChildCount();
		}

		if (parent_parent_node && strcmp(parent_parent_node->GetLabel(), "system") == 0)
		{
			num_columns = 100;
			//		num_columns = parent_node->GetChildCount();
		}
#endif
		return num_columns;
	}
	
	int rowCount(const QModelIndex &parent) const Q_DECL_OVERRIDE
	{
		TrackerNodeIF *node = trackerNode(parent);
		int num_rows;
		if (strcmp(node->GetLabel(), "system")==0)
		{
			num_rows = 2;
		}
		else
		{
			num_rows = node->GetChildCount();
		}

		return num_rows;
	}

};



