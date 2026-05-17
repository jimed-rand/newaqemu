#ifndef REGEXP_COMPAT_H
#define REGEXP_COMPAT_H

#include <QRegularExpression>
#include <QString>
#include <QStringList>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QRegularExpressionValidator>
#endif

class RegExpCompat
{
public:
	enum PatternSyntax { RegExp, Wildcard, FixedString };

	RegExpCompat();
	explicit RegExpCompat( const QString &pattern, PatternSyntax syntax = RegExp );

	void setPattern( const QString &pattern );
	void setPatternSyntax( PatternSyntax syntax );
	void setCaseSensitivity( Qt::CaseSensitivity cs );

	bool exactMatch( const QString &str ) const;
	int indexIn( const QString &str, int offset = 0 ) const;
	QStringList capturedTexts() const;
	QRegularExpression regularExpression() const;

private:
	void rebuild();
	bool matchInternal( const QString &str, int offset = 0 ) const;

	mutable QString m_lastSubject;
	mutable QRegularExpressionMatch m_lastMatch;

	QString m_pattern;
	PatternSyntax m_syntax = RegExp;
	Qt::CaseSensitivity m_caseSensitivity = Qt::CaseSensitive;
	QRegularExpression m_re;
};

QString replaceWithRegExp( QString subject, const RegExpCompat &rx, const QString &after );
QString removeWithRegExp( QString subject, const RegExpCompat &rx );
int indexOfRegExp( const QString &subject, const RegExpCompat &rx, int offset = 0 );

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
class RegExpCompatValidator : public QRegularExpressionValidator
{
	public:
		explicit RegExpCompatValidator( const RegExpCompat &rx, QObject *parent = nullptr );
};
#endif

#endif
