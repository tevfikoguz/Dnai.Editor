#ifndef FLOW_H
#define FLOW_H

#include "resourcesnode.h"
#include "baselinkable.h"
#include "linkablebezieritem.h"

namespace duly_gui
{
	namespace views
	{
		class FlowBackend : public BaseLinkable
		{
		public:
			explicit FlowBackend(DulyResources::FlowType t, QQuickItem *parent);

			/**
			* \brief return the IOType
			*/
			DulyResources::FlowType getType() const;

			Link *connect(ALinkable* linkable, BezierCurve* curve) override;

		protected:
			DulyResources::FlowType m_type;
		};

		class Flow : public LinkableBezierItem
		{
			Q_OBJECT
				Q_PROPERTY(DulyResources::FlowType type READ type WRITE setType NOTIFY typeChanged)

		public:
			explicit Flow(QQuickItem *parent = nullptr);
			virtual QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;

		public:
			DulyResources::FlowType type() const { return m_type; }

		public:
			void setType(DulyResources::FlowType t);
			/**
			* \brief Override componentComplete, and init some values
			*/
			virtual void componentComplete() override;

			virtual QPointF getCanvasPos() const override;

			virtual const QColor &colorLink() const override;

			/**
			* \brief Override findIo, return the IO under the point p of the Node n
			* \param n
			* \param p
			* \return Io *
			*/
			virtual LinkableBezierItem *findLinkableBezierItem(GenericNode *n, const QPointF &p) override;
			virtual void updateLink() override;
			virtual GenericNode *getNode() const override;

			virtual void setHover() override;
			virtual void setLink(Link *) override;

		signals:
			void typeChanged(DulyResources::FlowType t);

		private:
			DulyResources::FlowType m_type;

		protected:
			void mousePressEvent(QMouseEvent* event) override;
			virtual void afterRealease(Link *l) override;
		};
	}
}


#endif // FLOW_H
