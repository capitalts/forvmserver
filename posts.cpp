#include "post.h"

Post::Post(QObject *parent) : QObject(parent)
{

}

void Post::setText(QString text)
{
    m_text = text;
}

QString Post::text()
{
    return m_text;
}

QString Post::title()
{
    return
}

void Post::setTitle(QString title)
{
    m_title = title;
}

void Post::setArticles(QList<ArticleReader*> articles)
{
   m_articles = articles;
}

QList<ArticleReader*> Post::article()
{
    return m_articles;
}

void Post::setIcon(QImage icon)
{
    m_icon = icon;
}

QImage Post::icon()
{
    return m_icon;
}

int Post::agree()
{
    return m_agree;
}

void Post::setAgree()
{
    m_agree++;
}

int Post::disagree()
{
    return m_disagree;
}

void Post::setDisagree()
{
    m_disagree++;
}



