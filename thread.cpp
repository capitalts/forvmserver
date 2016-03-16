#include "thread.h"

Thread::Thread(QObject *parent) : QObject(parent)
{

}

QList<ArticleReader *> Thread::articles()
{
    return m_articles;
}

QList<Post *> Thread::posts()
{
    return m_posts;
}

void Thread::addArticle(QUrl url)
{
    ArticleReader article;
    article.startRequest(url);
    m_articles.append(article);
}

void Thread::addPost(Post post)
{
    Post post;
    post.setText();
}

void Thread::newThread(QList<ArticleReader> opArticles, Post OP)
{
    file = new QFile(OP.title().replace(" ", '_') + ".xml");
    out = new QXmlStreamWriter(&file);

    out->writeStartDocument();
    out->setAutoFormatting(true);
    out->writeStartElement("Thread");
    for(int i = 0; i < opArticles.length(); i++){
        out->writeStartElement("Article");
        out->writeTextElement("source", opArticles[i].articleSource.toString());
        out->writeEndElement();
    }
    out->writeStartElement("Post");
    out->writeTextElement("Icon", OP.icon());
    out->writeTextElement("Text", OP.text());
    out->writeTextElement("Agree", OP.agree());
    out->writeTextElement("Disagree", OP.disagree());
    out->writeEndElement();
    out->writeEndElement();
    out->writeEndDocument();


}

