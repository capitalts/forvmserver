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
    qDebug() << "connected readyRead and disconnected";
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnect()));


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
    QDomNodeList outArtList = outArticles.elementsByTagName("artilce");
    for(int i = 0; i < inArtList.size(); i++){
        QDomElement inBias = inArtList.at(i).firstChildElement("bias");
        QDomElement outBias = outArtList.at(i).firstChildElement("bias");
        if(inBias.firstChild().toElement().text() !=
                outBias.firstChild().toElement().text()){
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
    QDomNodeList outArtList = outArticles.elementsByTagName("artilce");
    for(int i = 0; i < inArtList.size(); i++){
        QDomElement inFair = inArtList.at(i).firstChildElement("fair");
        QDomElement outFair = outArtList.at(i).firstChildElement("fair");
    if(inFair.firstChild().toElement().text() != outFair.firstChild().toElement().text()){
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

void updater::readyRead(){
    qDebug() << "ReadyRead";
    QByteArray Data = socket->readAll();
    inDoc.setContent(Data);
    qDebug() << inDoc.firstChild().toElement().text();
    QDomElement thread = inDoc.firstChildElement("thread");
    QString header = thread.firstChildElement("header").text();
    fileName = thread.firstChildElement("fileName").firstChild().toElement().text();
    QFile inComingFile("/home/tory/QtProjects/ForvmServer/" + fileName);
    QFile outGoingFile("/home/tory/QtProjects/ForvmServer/" + fileName);
    outDoc.setContent(&inComingFile);
    outGoingFile.open(QIODevice::ReadWrite | QIODevice::Truncate);
    if(header == "addArticles"){
            addArticles();
            outGoingFile.write(outDoc.toByteArray());
    }else if(header == "biasChanged"){
            qDebug() << "Bias Changed";
            biasChange();
            qDebug() << "Writing to file";
            outGoingFile.write(outDoc.toByteArray());
    }else if(header == "fairChanged"){
            fairChange();
            outGoingFile.write(outDoc.toByteArray());
     }else if(header == "post"){
            post();
            outGoingFile.write(outDoc.toByteArray());
     }



    Data = outGoingFile.readAll();

    outGoingFile.close();

    qDebug() << "Sending";
    socket->write(Data);
}

void updater::disconnect(){
    qDebug()<< socketDescriptor << "disconnected" ;

    socket->deleteLater();
    exit(0);
}

