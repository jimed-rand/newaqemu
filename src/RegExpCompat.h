#ifndef REGEXP_COMPAT_H
#define REGEXP_COMPAT_H

#include <QRegularExpression>
#include <QString>
#include <QStringList>

class RegExpCompat
{
public:
	enum PatternSyntax { RegExp, Wildcard, FixedString };

	RegExpCompat();
	explicit RegExpCompat( const QString &pattern, PatternSyntax syntax = RegExp );

	void setPattern( const QString &pattern );
	void setPatternSyntax( PatternSyntax syntax );

	bool exactMatch( const QString &str ) const;
	int indexIn( const QString &str, int offset = 0 ) const;
	QStringList capturedTexts() const;

private:
	bool matchInternal( const QString &str, int offset = 0 ) const;

	mutable QString m_lastSubject;
	mutable QRegularExpressionMatch m_lastMatch;

	QString m_pattern;
	PatternSyntax m_syntax = RegExp;
	QRegularExpression m_re;
};

#endif
