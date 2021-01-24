
/*
    treeitem.cpp

    A container for items of data supplied by the simple tree model.
*/

#include <QStringList>

#include "treeitem.h"

//! [0]
TreeItem::TreeItem(const QList<QVariant> &data, TreeItem *parent)
{
    m_parentItem = parent;
    m_itemData = data;
}
//! [0]

//! [1]
TreeItem::~TreeItem()
{
    qDeleteAll(m_childItems);
}
//! [1]

//! [2]
void TreeItem::appendChild(TreeItem *item)
{
    m_childItems.push_back(item);
}
//! [2]

//! [3]
TreeItem *TreeItem::child(int row)
{
	if (row >= 0)
		return m_childItems[row];
	else
		return nullptr;
}
//! [3]

//! [4]
int TreeItem::childCount() const
{
	return m_childItems.size();
}
//! [4]

//! [5]
int TreeItem::columnCount() const
{
    // QVariant is a union.  these model
    // view things require values as QVariants I think
    //
    //QList<QVariant> m_itemData;

    return m_itemData.count();
}
//! [5]

//! [6]
QVariant TreeItem::data(int column) const
{
    return m_itemData.value(column);
}
//! [6]

//! [7]
TreeItem *TreeItem::parentItem()
{
    return m_parentItem;
}
//! [7]

//! [8]
int TreeItem::row() const
{
	if (m_parentItem)
	{
		int index = -1;
		for (int i = 0; i < m_parentItem->m_childItems.size(); i++)
		{
			if (m_parentItem->m_childItems[i] == this)
			{
				index = i;
				break;
			}
		}
	}
    
    return 0;
}
//! [8]
