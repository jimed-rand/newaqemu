#include "RegExpCompat.h"

RegExpCompat::RegExpCompat()
{
	rebuild();
}

RegExpCompat::RegExpCompat( const QString &pattern, PatternSyntax syntax )
	: m_pattern( pattern )
	, m_syntax( syntax )
{
	rebuild();
}

void RegExpCompat::setPattern( const QString &pattern )
{
	m_pattern = pattern;
	rebuild();
}

void RegExpCompat::setPatternSyntax( PatternSyntax syntax )
{
	m_syntax = syntax;
	rebuild();
}

void RegExpCompat::rebuild()
{
	QRegularExpression::PatternOptions opts = QRegularExpression::NoPatternOption;
	QString pat = m_pattern;

	switch( m_syntax )
	{
	case Wildcard:
		pat = QRegularExpression::escape( m_pattern );
		pat.replace( QStringLiteral( "\\*" ), QStringLiteral( ".*" ) );
		pat.replace( QStringLiteral( "\\?" ), QStringLiteral( "." ) );
		pat = QStringLiteral( "^" ) + pat + QStringLiteral( "$" );
		break;
	case FixedString:
		pat = QRegularExpression::escape( m_pattern );
		pat = QStringLiteral( "^" ) + pat + QStringLiteral( "$" );
		break;
	case RegExp:
	default:
		if( ! pat.startsWith( '^' ) )
			pat.prepend( '^' );
		if( ! pat.endsWith( '$' ) )
			pat.append( '$' );
		break;
	}

	m_re.setPattern( pat, opts );
}

bool RegExpCompat::matchInternal( const QString &str, int offset ) const
{
	m_lastSubject = str;
	m_lastMatch = m_re.match( str, offset );
	return m_lastMatch.hasMatch();
}

bool RegExpCompat::exactMatch( const QString &str ) const
{
	if( ! matchInternal( str ) )
		return false;
	return m_lastMatch.capturedStart() == 0
		&& m_lastMatch.capturedLength() == str.length();
}

int RegExpCompat::indexIn( const QString &str, int offset ) const
{
	if( ! matchInternal( str, offset ) )
		return -1;
	return m_lastMatch.capturedStart();
}

QStringList RegExpCompat::capturedTexts() const
{
	QStringList out;
	if( ! m_lastMatch.hasMatch() )
		return out;

	out << m_lastMatch.captured( 0 );
	for( int i = 1; i <= m_lastMatch.lastCapturedIndex(); ++i )
		out << m_lastMatch.captured( i );
	return out;
}
