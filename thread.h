#ifndef THREAD_H
#define THREAD_H
#include "articlereader.h"
#include <QObject>
#include <QList>
#include "posts.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QFile>
class Thread : public QObject
{
    Q_OBJECT

public:
    explicit Thread(QObject *parent = 0);
    QList<ArticleReader*> articles();
    QList<Posts*> posts();
    void addArticle(QUrl url);
    void addPost(Posts post);
    void newThread(QList<ArticleReader> opArticles, Post OP);

signals:
    void articlesChanged();
    void postsChanged();
public slots:


private:
    QList<ArticleReader*> m_articles;
    QList<Posts*> m_posts;
    QXmlStreamReader in;
    QXmlStreamWriter* out;
    QFile file;
};
#endif // THREAD_H
