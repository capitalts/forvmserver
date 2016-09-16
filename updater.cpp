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
        if(inArtList.at(outArtList.size()).childNodes().size() != 6){
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
    QFile mainThreads("/home/tory/QtProjects/ForvmServerXMLFiles/MainThreads.xml");
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
    qDebug() << "inPost" << inPost.isNull();
    inPost.appendChild(postNumber);
    qDebug() << "outPosts" << outThread.firstChildElement("posts").isNull();
    QDomElement outPosts = outThread.firstChildElement("posts");
    outPosts.appendChild(inPost);
    mainThreads.close();

}

void updater::newThread()
{
      QFile file("/home/tory/QtProjects/ForvmServerXMLFiles/" + fileName);
      file.open(QIODevice::ReadWrite);
      QDomElement thread = outDoc.firstChildElement("thread");
      QDomElement header = thread.firstChildElement("header");
      thread.removeChild(header);
      QDomElement article = thread.firstChildElement("articles").firstChildElement("article");
      QDomElement outPost = thread.firstChildElement("posts").firstChildElement("post");
      thread.firstChildElement("posts").removeChild(outPost);
      thread.firstChildElement("articles").removeChild(article);
      post();
      file.write(outDoc.toByteArray());
      file.close();
//      qDebug() << outDoc.toString();
      addArticles();
}


void updater::bytesWritten(qint64 bytes)
{
    qDebug() << bytes << " bytes written...";
}

void updater::readyRead(){
    qDebug() << "ReadyRead";
    QByteArray Data = socket->readAll();
    inDoc.setContent(Data);
    QDomElement thread = inDoc.firstChildElement("threads");
    if(thread.isNull()){
        thread = inDoc.firstChildElement("thread");
    }
    QString header = thread.firstChildElement("header").text();
    fileName = thread.firstChildElement("fileName").text();
    qDebug() << "fileName" << fileName;
    QFile file("/home/tory/QtProjects/ForvmServerXMLFiles/" + fileName);
    file.open(QIODevice::ReadWrite);
    outDoc.setContent(&file);
    if(header == "article"){
                qDebug() << "article";
                addArticles();

    }else if(header == "New Thread"){
        outDoc.setContent(Data);
        newThread();

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
        qDebug() << "update";
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
        QFile file("/home/tory/QtProjects/ForvmServerXMLFiles/" + fileName);
        file.open(QIODevice::ReadWrite);

        file.resize(0);
        QDomElement inRoot = inDoc.firstChildElement("thread");
        QDomElement outRoot = outDoc.firstChildElement("thread");
        QDomElement outArticles = outRoot.firstChildElement("articles");
        QDomElement inArticles = inRoot.firstChildElement("articles");
        QDomNodeList inArtList = inArticles.elementsByTagName("article");
        QDomNodeList outArtList = outArticles.elementsByTagName("article");
        findTitleAndImgSrc(reply->readAll().data(), inArtList.at(outArtList.size()).firstChildElement("source").text());
        qDebug() << "imgSource" << imageSource;
        qDebug() << "arttitle:" << artTitle;
        QDomElement imgSource = outDoc.createElement("imgSource");
        QDomElement title = outDoc.createElement("title");
        QDomElement outArt = outDoc.createElement("article");
        imgSource.appendChild(inDoc.createTextNode(imageSource));
        title.appendChild(inDoc.createTextNode(artTitle));
        outArt.appendChild(inArtList.at(outArtList.size()).firstChildElement("source"));
        outArt.appendChild(inArtList.at(outArtList.size()).firstChildElement("fair"));
        outArt.appendChild(inArtList.at(outArtList.size()).firstChildElement("bias"));
        outArt.appendChild(title);
        outArt.appendChild(imgSource);
        outArticles.appendChild(outArt);
        qDebug() << "header" << inRoot.firstChildElement("header").text();
        if(inRoot.firstChildElement("header").text() == "New Thread"){
            QFile mainThreads("/home/tory/QtProjects/ForvmServerXMLFiles/MainThreads.xml");

            mainThreads.open(QIODevice::ReadWrite);
            QDomDocument mainDoc;
            mainDoc.setContent(&mainThreads);
            mainThreads.resize(0);
            QDomElement threads = mainDoc.firstChildElement("threads");
            QDomElement newThread = mainDoc.createElement("thread");
            QDomElement source = mainDoc.createElement("source");
            QDomElement threadTitle = mainDoc.createElement("title");
            QDomElement imageSrc = mainDoc.createElement("imageSrc");
            if(inRoot.firstChildElement("head").text().isEmpty()){
                threadTitle.appendChild(mainDoc.createTextNode(artTitle));
            }else{
                threadTitle.appendChild(mainDoc.createTextNode(inRoot.firstChildElement("head").text()));
            }
            imageSrc.appendChild(mainDoc.createTextNode(imgSource.text()));
            source.appendChild(mainDoc.createTextNode(fileName));
            newThread.appendChild(threadTitle);
            newThread.appendChild(imageSrc);
            newThread.appendChild(source);
            threads.insertBefore(newThread, threads.firstChildElement("thread"));
            mainThreads.write(mainDoc.toByteArray());

        }
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
    artTitle.clear();
    imageSource.clear();
    reply->deleteLater();
}

void updater::findTitleAndImgSrc(QString html, QString artSource)
{
    html = html.simplified();
    artTitle = html.mid(html.indexOf("<title>") + 7, html.indexOf("</title>")-html.indexOf("<title>")-7);
    QString articleImageHtml = html.mid(html.indexOf("og:image"));
    imageSource = articleImageHtml.mid(articleImageHtml.indexOf("http://"), articleImageHtml.indexOf("jpg") + 3
                                         -articleImageHtml.indexOf("http://"));


}


