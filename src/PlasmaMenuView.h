
#ifndef _PLASMA_MENU_ITEM
#include <QString>
#include <QList>
#include <QCryptographicHash>

#include <KDesktopFile>
#include <KServiceGroup>
#include <KService>

#include "tpl_engine/TemplateMap.h"
#include "tpl_engine/TemplateList.h"

class PlasmaMenuItem: public TemplateMap {
    public:
        PlasmaMenuItem(): TemplateMap() {
            m_type="PlasmaMenuItem";
        }
        PlasmaMenuItem(const QString &_icon, const QString &_caption) {
            setCaption(_caption);
            setIcon(_icon);
        }
        void setCaption(const QString &_caption){
            put("caption",_caption);
        }
        void setIcon(const QString &_icon){
            put("icon",_icon);
        }
        QString getIcon() { return getQString("icon"); }
        QString getCaption () { return getQString("caption"); }
        QString getType() { return m_type; }

    protected:    
        QString m_type;
};

class PlasmaMenuFolder : public PlasmaMenuItem {
    public:
        PlasmaMenuFolder(){
            m_type = "PlasmaMenuFolder";
            put("menuItems", new TemplateList());
        }
        PlasmaMenuFolder(const QString &_icon, const QString &_caption) : PlasmaMenuItem(_icon, _caption){
            m_type = "PlasmaMenuFolder";
            put("menuItems", new TemplateList());
        }
        void add(PlasmaMenuItem *);
        void setGenericName(const QString &_genericName){
            put("genericName",_genericName);
        }
        void setComment(const QString &_comment){
            put("comment",_comment);
        }
        void setId(const QString &_id){
            put("id",_id);
        }
        void setFullId(const QString &_fullId){
            put("fullId", _fullId);
        }
        void setDirectoryFile(const QString &_directoryFile){
            put("directoryFile", _directoryFile);
        }
        QString getGenericName(){
            return getQString("genericName");
        }
        QString getComment(){
            return getQString("comment");
        }
        QString getId(){
            return getQString("id");
        }
        QString getFullId(){
            return getQString("fullId");
        }
        TemplateList* getMenuItems(){
            return static_cast<TemplateList*>(get("menuItems"));
        }
};

class PlasmaMenuEntry : public PlasmaMenuItem {
    public:
        explicit PlasmaMenuEntry(const KService::Ptr &_service,
                                 KDesktopFile *_df = nullptr): service(_service)
                                                             , m_desktopFile(_df){
            m_type = "PlasmaMenuEntry";
            setCaption(service->name());
            setDescription(service->genericName());
            setIcon(service->icon());
            if (service->isApplication()){
                setExec(service->exec());
            }
            setPath(service->workingDirectory());
        }
        KDesktopFile *desktopFile(){
            if (!m_desktopFile) {
                m_desktopFile = new KDesktopFile(service->entryPath());
            }
            return m_desktopFile;
        }

        void setDescription(QString _description){
            put("description", _description);
        }

        QString getDescription(){
            return getQString("description");
        }

        void setExec(QString _exec){
            put ("exec", _exec);
            QCryptographicHash hash(QCryptographicHash::Md5);
            hash.addData(_exec.toLatin1());
            put ("uid", hash.result().toHex());
        }

        QString getExec(){
            return getQString("exec");
        }

        void setPath(QString _path){
            put ("path", _path);
        }

        QString getPath(){
            return getQString("path");
        }
    private:
        KService::Ptr service;
        KDesktopFile *m_desktopFile;
};

class PlasmaMenuView {
    public:
        PlasmaMenuView(){
            m_rootElement = new PlasmaMenuFolder();
        }
        void readMenuFolderInfo(PlasmaMenuFolder *folderInfo = 0,
                                KServiceGroup::Ptr folder = KServiceGroup::Ptr(),
                                const QString &prefix = QString());
        

        void debugOutput();
        PlasmaMenuEntry* findEntryByUid(QString _uid,
                                     KServiceGroup::Ptr folder = KServiceGroup::Ptr());
        TemplateObject* templateData();

    private:
        PlasmaMenuFolder *m_rootElement;
};

#endif