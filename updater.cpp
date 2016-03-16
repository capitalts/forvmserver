#include "updater.h"

updater::updater(int ID, QObject *parent) : QThread(parent)
{
    this->socketDescriptor = ID;

}

void updater::run(){

    socket = new QTcpSocket();
    if(!socket->setSocketDescriptor(this->socketDescriptor)){
        emit error(socket->error());
        return;
    }

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()), Qt::DirectConnection);


    exec();
}

void updater::addArticles(){

    QDomElement inRoot = inDoc.firstChildElement("root");
    QDomElement outRoot = outDoc.firstChildElement("root");
    QDomElement outArticles = outRoot.firstChildElement("articles");
    QDomElement inArticles = inRoot.firstChildElement("articles");
    QDomNodeList inArtList = inArticles.elementsByTagName("article");
    QDomNodeList outArtList = outArticles.elementsByTagName("artilce");
    if(inArtList.size() != outArtList.size()){
    for(int i = 0; i < inArtList.size(); i++){
        if(inArtList.at(i).childNodes().size() != 5){
            QString art = inArtList.at(i).firstChildElement("source").firstChild().nodeValue();
            reader.startRequest(art);
            QDomElement imgSource = inDoc.createElement("imgSource");
            QDomElement title = inDoc.createElement("title");
            imgSource.appendChild(inDoc.createTextNode(reader.getImageSource()));
            title.appendChild(inDoc.createTextNode(reader.getArticleTitle()));
            inArtList.at(i).appendChild(imgSource);
            inArtList.at(i).appendChild(title);
            outArticles.appendChild(inArtList.at(i));
            }
        }
    }
}

void updater::biasChange()
{
    QDomElement inRoot = inDoc.firstChildElement("root");
    QDomElement outRoot = outDoc.firstChildElement("root");
    QDomElement outArticles = outRoot.firstChildElement("articles");
    QDomElement inArticles = inRoot.firstChildElement("articles");
    QDomNodeList inArtList = inArticles.elementsByTagName("article");
    QDomNodeList outArtList = outArticles.elementsByTagName("artilce");
    for(int i = 0; i < inArtList.size(); i++){
        QDomElement inBias = inArtList.at(i).firstChildElement("bias");
        QDomElement outBias = outArtList.at(i).firstChildElement("bias");
        if(inBias.firstChild() !=
                outBias.firstChild()){
            outBias.replaceChild(inBias.firstChild(), outBias.firstChild());
        }
    }
}

void updater::fairChange()
{
    QDomElement inRoot = inDoc.firstChildElement("root");
    QDomElement outRoot = outDoc.firstChildElement("root");
    QDomElement outArticles = outRoot.firstChildElement("articles");
    QDomElement inArticles = inRoot.firstChildElement("articles");
    QDomNodeList inArtList = inArticles.elementsByTagName("article");
    QDomNodeList outArtList = outArticles.elementsByTagName("artilce");
    for(int i = 0; i < inArtList.size(); i++){
        QDomElement inFair = inArtList.at(i).firstChildElement("fair");
        QDomElement outFair = outArtList.at(i).firstChildElement("fair");
    if(inFair.firstChild() != outFair.firstChild()){
            outFair.replaceChild(inFair.firstChild(), outFair.firstChild());
        }
    }
}

void updater::post()
{
    QDomElement inRoot = inDoc.firstChildElement("root");
    QDomElement outRoot = outDoc.firstChildElement("root");
    QDomNodeList inPosts = inRoot.firstChildElement("posts").elementsByTagName("post");
    QDomElement outPosts = outRoot.firstChildElement("posts");
    outPosts.appendChild(inPosts.at(inPosts.size()-1));

}

void updater::readyRead(){
    QByteArray Data = socket->readAll();
    inDoc.setContent(Data);

    QDomElement inRoot = inDoc.firstChildElement("root");
    QString header = inRoot.firstChild().nodeValue();
    QFile inComingFile(inRoot.tagName() + ".xml");
    QFile outGoingFile(inRoot.tagName() + ".xml");
            outDoc.setContent(&inComingFile);
            switch(header){

            case "addArticles":
                    addArticles();
                    outGoingFile.open(QIODevice::ReadWrite | QIODevice::Truncate);
                    outGoingFile.write(outDoc.toByteArray());
                break;
            case "biasChanged":
                    biasChange();
                    outGoingFile.open(QIODevice::ReadWrite | QIODevice::Truncate);
                    outGoingFile.write(outDoc.toByteArray());
                break;
            case "fairChanged":
                    fairChange();
                    outGoingFile.open(QIODevice::ReadWrite | QIODevice::Truncate);
                    outGoingFile.write(outDoc.toByteArray());
                break;
            case "post":
                    post();
                    outGoingFile.open(QIODevice::ReadWrite | QIODevice::Truncate);
                    outGoingFile.write(outDoc.toByteArray());
                break;

            }

    Data = outGoingFile.readAll();
    outGoingFile.close();


    socket->write(Data);
}

void updater::disconnected(){
    qDebug()<< socketDescriptor << "disconnected" ;

    socket->deleteLater();
    exit(0);
}

