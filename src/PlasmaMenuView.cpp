#include "PlasmaMenuView.h"

#include <QDebug>
#include <KSycoca>

void PlasmaMenuView::readMenuFolderInfo (PlasmaMenuFolder *folderInfo,
                                         KServiceGroup::Ptr folder,
                                         const QString &prefix) {
	if (!folderInfo) {
		folderInfo = m_rootElement;
		folder = KServiceGroup::root();
	}

	if (!folder ||
	    !folder->isValid() ||
		folder->noDisplay()){
		return;
	}

	folderInfo->setCaption(folder->caption());
	folderInfo->setComment(folder->comment());
	folderInfo->setDirectoryFile(folder->directoryEntryPath());
	folderInfo->setIcon(folder->icon());
	QString id = folder->relPath();
	int i = id.lastIndexOf(QLatin1Char('/'), -2);
	id = id.mid(i + 1);
	folderInfo->setId(id);
	folderInfo->setFullId(prefix + id);

	for (const KSycocaEntry::Ptr &e : folder->entries(true, false, false, false)){
		if (e->isType(KST_KServiceGroup)){
			const KServiceGroup::Ptr serviceGroup(static_cast<KServiceGroup *>(e.data()));
			if (serviceGroup->noDisplay()){
				continue;
			}
			PlasmaMenuFolder *subFolder = new PlasmaMenuFolder();
			readMenuFolderInfo(subFolder, serviceGroup, folderInfo->getFullId());
			if (subFolder->getMenuItems()->size()>0){
			    folderInfo->add(subFolder);
			}
		} else if (e->isType(KST_KService)) {
			const KService::Ptr service(static_cast<KService *>(e.data()));
			if (service->noDisplay() || !service->isApplication()){
				continue;
			}
			folderInfo->add(new PlasmaMenuEntry(service));
		}
	}
}

void PlasmaMenuView::debugOutput(){
    qDebug().noquote() << m_rootElement->toString().c_str();
}

TemplateObject* PlasmaMenuView::templateData(){
	return m_rootElement;
}

void PlasmaMenuFolder::add(PlasmaMenuItem *_menuItem){
    static_cast<TemplateList *>(get("menuItems"))->push(*_menuItem);
}

PlasmaMenuEntry* PlasmaMenuView::findEntryByUid(QString _uid,
                                                KServiceGroup::Ptr folder){
    if (!folder ||
	    !folder->isValid() ||
		folder->noDisplay()){
		qDebug() << "Not searching anything here";
		return 0;
	}
 	for (const KSycocaEntry::Ptr &e : folder->entries(true, false, false, false)){
		if (e->isType(KST_KServiceGroup)){
			const KServiceGroup::Ptr serviceGroup(static_cast<KServiceGroup *>(e.data()));
			if (serviceGroup->noDisplay()){
				continue;
			}
			PlasmaMenuEntry *entry = findEntryByUid(_uid, serviceGroup);
			if (entry!=0){
			    return entry;
			}
		} else if (e->isType(KST_KService)) {
			const KService::Ptr service(static_cast<KService *>(e.data()));
			if (service->noDisplay() || !service->isApplication()){
				continue;
			}
			PlasmaMenuEntry *entry = new PlasmaMenuEntry(service);
			if (entry->get("uid")->toString().c_str()==_uid){
				qDebug() << "Found Entry";
				return entry;
			}
		}
	}   


	return 0;
}