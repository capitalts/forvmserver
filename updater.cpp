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
    qnam = new QNetworkAccessManager(this);
    connect(qnam, SIGNAL(finished(QNetworkReply*)),this, SLOT(parseArticle()));
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
    QDomNodeList outArtList = outArticles.elementsByTagName("article");
    qDebug() << inArtList.at(outArtList.size()).firstChildElement("source").text();
    if(inArtList.size() != outArtList.size()){
        if(inArtList.at(outArtList.size()).childNodes().size() != 5){
            QUrl art(inArtList.at(outArtList.size()).firstChildElement("source").text());
            reply = qnam->get(QNetworkRequest(art));
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
    QFile mainThreads("/home/tory/QtProjects/ForvmServer/ForvmServerXMLFiles/MainThreads.xml");
    mainThreads.open(QIODevice::ReadWrite);
    QDomDocument doc;
    doc.setContent(&mainThreads);
    mainThreads.resize(0);
    QDomElement ele = doc.firstChildElement("threads");
    QDomElement postNumEle = ele.firstChildElement("currentPostNumber");
    int num = postNumEle.text().toInt();
    num += 1;
    postNumEle.replaceChild(doc.createTextNode(QString::number(num)),postNumEle.firstChild());
    mainThreads.write(doc.toByteArray());
    QDomElement inThread= inDoc.firstChildElement("thread");
    QDomElement outThread = outDoc.firstChildElement("thread");
    QDomNodeList inPosts = inThread.firstChildElement("posts").elementsByTagName("post");
    QDomElement postNumber = inDoc.createElement("postNumber");
    QDomElement inPost = inPosts.at(inPosts.size()-1).toElement();
    postNumber.appendChild(inDoc.createTextNode(QString::number(num)));
    inPost.appendChild(postNumber);
    QDomElement outPosts = outThread.firstChildElement("posts");
    outPosts.appendChild(inPost);

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
    if(header == "article"){
                qDebug() << "article";
                addArticles();

    }
    else{
        if(header == "biasChanged"){
                file.resize(0);
                qDebug() << "Bias Changed";
                biasChange();
                qDebug() << "Writing to file";
                file.write(outDoc.toByteArray());
        }else if(header == "fairChanged"){
                file.resize(0);
                qDebug() << "fairChanged";
                fairChange();
                qDebug() << "Writing to file";
                file.write(outDoc.toByteArray());
         }else if(header == "post"){
                file.resize(0);
                qDebug() << "post";
                post();
                file.write(outDoc.toByteArray());
        }
        QByteArray newData = outDoc.toByteArray();
        if(socket->isOpen()){
            qDebug() << "Writing...";
            qDebug() << socket->write(newData);
            qDebug() << "Done Writing";
        }else{
           qDebug() << "Socket not open";
        }
     }
    file.close();
}

void updater::disconnect(){
    qDebug()<< socketDescriptor << "disconnected" ;
    qnam->deleteLater();
    socket->deleteLater();
    exit(0);
}

void updater::parseArticle()
{
    qDebug() << "parse article";
    if(reply->error()){
            qDebug() << "ERROR!";
            qDebug() << reply->errorString();
        }
    else{
        qDebug() << "read all";
        QFile file("/home/tory/QtProjects/ForvmServer/ForvmServerXMLFiles/" + fileName);
        file.open(QIODevice::ReadWrite);
        articleHtml.setContent(reply->readAll());
        qDebug() << "artHtml" << articleHtml.toString();
        QDomElement inRoot = inDoc.firstChildElement("thread");
        QDomElement outRoot = outDoc.firstChildElement("thread");
        QDomElement outArticles = outRoot.firstChildElement("articles");
        QDomElement inArticles = inRoot.firstChildElement("articles");
        QDomNodeList inArtList = inArticles.elementsByTagName("article");
        QDomNodeList outArtList = outArticles.elementsByTagName("article");
        QString articleTitle = articleHtml.firstChild().firstChildElement("head").elementsByTagName("title").at(0).toElement().text();
        QDomNodeList metaList = articleHtml.firstChild().firstChildElement("head").elementsByTagName("meta");
        QDomAttr content;
        qDebug() << "artTitle" << articleTitle;
        for(int i = 0; i < metaList.length(); i++){
            QDomAttr property = metaList.at(i).toElement().attributeNode("property");
            if(property.value() == "og:image"){
               content = metaList.at(i).toElement().attributeNode("content");
               break;
            }
        }
        QString imageSource = content.value();
        qDebug() << "imgSource" << imageSource;

        QDomElement imgSource = inDoc.createElement("imgSource");
        QDomElement title = inDoc.createElement("title");
        imgSource.appendChild(inDoc.createTextNode(imageSource));
        title.appendChild(inDoc.createTextNode(articleTitle));
        inArtList.at(outArtList.size()).appendChild(imgSource);
        inArtList.at(outArtList.size()).appendChild(title);
        outArticles.appendChild(inArtList.at(outArtList.size()));
        file.write(outDoc.toByteArray());
        QByteArray newData = outDoc.toByteArray();
        if(socket->isOpen()){
            qDebug() << "Writing...";
            qDebug() << socket->write(newData);
            qDebug() << "Done Writing";
        }else{
           qDebug() << "Socket not open";
        }
    }
    reply->deleteLater();
}

