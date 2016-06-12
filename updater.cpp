#include "updater.h"

updater::updater(qintptr ID, QObject *parent) : QThread(parent)
{
    this->socketDescriptor = ID;
}


void updater::start(){

    socket = new QTcpSocket();
    if(!socket->setSocketDescriptor(this->socketDescriptor)){
        emit error(socket->error());
        return;
    }
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnect()));
    connect(socket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));
    qDebug() << socketDescriptor << "connected";

    exec();
}

void updater::addArticles(){

    QDomElement inRoot = inDoc.firstChildElement("thread");
    QDomElement outRoot = outDoc.firstChildElement("thread");
    QDomElement outArticles = outRoot.firstChildElement("articles");
    QDomElement inArticles = inRoot.firstChildElement("articles");
    QDomNodeList inArtList = inArticles.elementsByTagName("article");
    QDomNodeList outArtList = outArticles.elementsByTagName("artilce");
    if(inArtList.size() != outArtList.size()){
    for(int i = 0; i < inArtList.size(); i++){
        if(inArtList.at(i).childNodes().size() != 5){
            QString art = inArtList.at(i).firstChildElement("source").firstChild().toElement().text();
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
    QDomElement inRoot = inDoc.firstChildElement("thread");
    QDomElement outRoot = outDoc.firstChildElement("thread");
    QDomElement outArticles = outRoot.firstChildElement("articles");
    QDomElement inArticles = inRoot.firstChildElement("articles");
    QDomNodeList inArtList = inArticles.elementsByTagName("article");
    QDomNodeList outArtList = outArticles.elementsByTagName("article");
    for(int i = 0; i < inArtList.size(); i++){
        QDomElement inBias = inArtList.at(i).firstChildElement("bias");
        QDomElement outBias = outArtList.at(i).firstChildElement("bias");
        if(inBias.text()!=outBias.text()){
            outBias.replaceChild(inBias.firstChild(), outBias.firstChild());
        }
    }
}

void updater::fairChange()
{
    QDomElement inThread = inDoc.firstChildElement("thread");
    QDomElement outThread = outDoc.firstChildElement("thread");
    QDomElement outArticles = outThread.firstChildElement("articles");
    QDomElement inArticles = inThread.firstChildElement("articles");
    QDomNodeList inArtList = inArticles.elementsByTagName("article");
    QDomNodeList outArtList = outArticles.elementsByTagName("article");
    for(int i = 0; i < inArtList.size(); i++){
        QDomElement inFair = inArtList.at(i).firstChildElement("fair");
        QDomElement outFair = outArtList.at(i).firstChildElement("fair");
    if(inFair.text() != outFair.text()){
            outFair.replaceChild(inFair.firstChild(), outFair.firstChild());
        }
    }
}

void updater::post()
{
    QDomElement inRoot = inDoc.firstChildElement("thread");
    QDomElement outRoot = outDoc.firstChildElement("thread");
    QDomNodeList inPosts = inRoot.firstChildElement("posts").elementsByTagName("post");
    QDomElement outPosts = outRoot.firstChildElement("posts");
    outPosts.appendChild(inPosts.at(inPosts.size()-1));

}

void updater::bytesWritten(qint64 bytes)
{
    qDebug() << bytes << " bytes written...";
}

void updater::readyRead(){
    qDebug() << "ReadyRead";
    QByteArray Data = socket->readAll();
    inDoc.setContent(Data);
    QDomElement thread = inDoc.firstChildElement("thread");
    QString header = thread.firstChildElement("header").text();
    fileName = thread.firstChildElement("fileName").text();
    qDebug() << "fileName" << fileName;
    QFile file("/home/tory/QtProjects/ForvmServer/ForvmServerXMLFiles/" + fileName);
    file.open(QIODevice::ReadWrite);
    outDoc.setContent(&file);
    file.resize(0);
    if(header == "addArticles"){
            addArticles();
            file.write(outDoc.toByteArray());
    }else if(header == "biasChanged"){
            qDebug() << "Bias Changed";
            biasChange();
            qDebug() << "Writing to file";
            file.write(outDoc.toByteArray());
    }else if(header == "fairChanged"){
            qDebug() << "fairChanged";
            fairChange();
            qDebug() << "Writing to file";
            file.write(outDoc.toByteArray());
     }else if(header == "post"){
            post();
            file.write(outDoc.toByteArray());
     }

    QByteArray newData = outDoc.toByteArray();
    file.close();
    if(socket->isOpen()){
        qDebug() << "Writing...";
        qDebug() << socket->write(newData);
        qDebug() << "Done Writing";
    }else{
       qDebug() << "Socket not open";
    }
}

void updater::disconnect(){
    qDebug()<< socketDescriptor << "disconnected" ;

    socket->deleteLater();
    exit(0);
}

