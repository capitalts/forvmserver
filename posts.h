#ifndef POST_H
#define POST_H

#include <QObject>
#include "articlereader.h"
#include <QImage>
#include <QStringList>
#include <QQmlListProperty>
class Posts : public QObject
{
    Q_OBJECT

public:
    explicit Posts(QObject *parent = 0);
    void setText(QString text);
    QString text();
    QString title();
    void setTitle(QString title);
    QList<ArticleReader*> article();
    void setArticles(QList<ArticleReader*> articles);
    void setIcon(QImage icon);
    QImage icon();
    int agree();
    void setAgree();
    int disagree();
    void setDisagree();

signals:
    void agreeChanged();
    void disagreeChanged();
public slots:

private:
    QList<ArticleReader*> m_articles;
    QString m_text;
    QString m_title;
    QImage m_icon;
    int m_agree;
    int m_disagree;

};

#endif // POST_H
