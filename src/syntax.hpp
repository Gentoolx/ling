/**
 * @file    syntax.hpp
 * @author  Wintermute Developers <wintermute-devel@lists.launchpad.net>
 * @date February 4, 2011, 12:07 AM
 * @license GPL3
 *
 * @legalese
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * @endlegalese
 */

#ifndef __SYNTAX_HPP__
#define __SYNTAX_HPP__

#include <string>
#include <QMap>
#include <QList>
#include <wntrdata.hpp>

using namespace std;
using namespace Wintermute::Data::Linguistics;

using std::string;
using Wintermute::Data::Linguistics::Lexical::Data;
using Wintermute::Data::Linguistics::Lexical::FlagMapping;

namespace Wintermute {
    namespace Linguistics {
        struct Link;
        struct Node;
        struct Binding;

        /**
         * @brief Represents a @c QVector of strings.
         * @typedef StringVector
         */
        typedef QList<string> StringList;
        /**
         * @brief Represents a @c QVector of @c Nodes;
         * @typedef NodeVector
         */
        typedef QList<Node*> NodeList;
        /**
         * @brief Represents a @c QVector of @c Links;
         * @typedef LinkVector
         */
        typedef QList<Link*> LinkList;

        /**
         * @brief An object representing the lexical and syntactic bindings of a word-symbol.
         *
         * Wintermute's natural language processing system uses nodes to represent individual word
         * symbols in lexical representation and in syntactical representation. Nodes are used to
         * represent either a collection or an individual set of symbols from natural text. We
         * use the term "word symbol" or just "symbol" to represent words in English or possibly
         * hieroglyphs in ancient Egypt, to ensure a transparency of meaning. Some Node objects may have
         * a single meaning, for example:
         * @dot
            digraph L {
                    graph [layout=dot rankdir=LR]

                    {Apple} -> ontoApple
            }
         * @enddot
         *
         * But some Node objects, like pronouns in the English and romance languages can expand to have many
         * meanings, as depicted:
         * @dot
            digraph L {
                    graph [layout=dot rankdir=TB]

                    {YouGroup YouAre YouSecond YouThird} ->  You
            }
         * @enddot
         *
         * We have a convience method, expand(), that splits a Node into the different meanings it represents.
         *
         * @class Node syntax.hpp "include/wntr/ling/syntax.hpp"
         * @internal
         */
        class Node : public QObject {
            Q_GADGET
            Q_PROPERTY(const QString id READ id)
            Q_PROPERTY(const QString locale READ locale)
            Q_PROPERTY(const QString symbol READ symbol)
            Q_PROPERTY(const QString value READ toString)
            Q_PROPERTY(const FlagMapping flags READ flags)
            Q_PROPERTY(const Data* data READ data)
            Q_ENUMS(FormatVerbosity)

            friend class Link;

            protected:
                Lexical::Data m_lxdt;

            public:
                /**
                 * @brief The format verbosity of a Node in text.
                 *
                 * The enumeration shown here represents the verbosity of the information
                 * generated when representing a @c Node as a string.
                 *
                 * @enum FormatVerbosity.
                 */
                enum FormatVerbosity {
                    FULL = 0, /**< Represents a full dump of the node's first flag, ontological mapping, and it's ID. */
                    MINIMAL, /**< Represents only the first flag's first letter. */
                    EXTRA /**< Represents the first flag.  */
                };

                /**
                 * @brief Null constructor.
                 * @fn Node
                 */
                Node( ) : m_lxdt() { }

                /**
                 * @brief Default constructor.
                 * @fn Node
                 * @param p_lxdt The Lexical::Data representing the internal data of the Node.
                 */
                explicit Node ( Lexical::Data p_lxdt ) : m_lxdt(p_lxdt) {
                    this->setProperty ("OriginalToken",symbol ());
                }

                /**
                 * @brief Copy constructor.
                 * @fn Node
                 * @param p_nd The node being copied.
                 */
                Node( const Node& p_nd ) : m_lxdt(p_nd.m_lxdt) {
                    this->setProperty ("OriginalToken",symbol ());
                }

                /**
                 * @brief Deconstructor.
                 * @fn ~Node
                 */
                ~Node() { }

                /**
                 * @brief Obtains the ID of the Node.
                 * @fn id
                 * @return The internal ID of the node, or an empty QString.
                 * @note It's very unlikely that you'd get an empty QString, as an update to the symbol would
                 *       generate an ID, and in order to obtain the internal Lexical::Data, the ID is required.
                 * @see Node(Lexical::Data p_lxdt)
                 * @see Lexical::Data
                 */
                Q_INVOKABLE inline const QString id() const {
                    return m_lxdt.id ();
                }

                /**
                 * @brief Obtains the locale of the Node.
                 * @fn locale
                 * @return The locale of the Node, or an empty QString.
                 * @note It's very unlikely that you'd get an empty QString, due to the fact
                 *        the internal Lexical::Data requires a value for the locale.
                 * @see Node(Lexical::Data p_lxdt)
                 * @see Lexical::Data
                 */
                Q_INVOKABLE inline const QString locale() const {
                    return m_lxdt.locale ();
                }

                /**
                 * @brief Obtains the symbol of the Node.
                 * @fn symbol
                 * @return The symbol of the Node, or an empty QString.
                 */
                Q_INVOKABLE inline const QString symbol() const {
                    return m_lxdt.symbol ();
                }

                /**
                 * @brief Obtains the flags of the Node.
                 * @fn flags
                 * @return The flags of the Node.
                 */
                Q_INVOKABLE inline const FlagMapping flags() const {
                    return m_lxdt.flags ();
                }

                /**
                 * @brief Obtains the internal Lexical::Data object.
                 * @fn data
                 * @return The Lexical::Data defining this node.
                 */
                Q_INVOKABLE inline const Lexical::Data* data() const {
                    return &m_lxdt;
                }

                /**
                 * @brief Determines if the Node is 'flat'.
                 *
                 * A Node is considered 'flat' if and only if the size of the FlagMapping returned
                 * by flags() is 1.
                 *
                 * @fn isFlat
                 * @return 'true' if flags().size == 1, returns 'false' otherwise.
                 */
                Q_INVOKABLE inline const bool isFlat() const {
                    return this->flags ().size () == 1;
                }

                /**
                 * @brief Determines if the Node is 'psuedo'.
                 *
                 * Some nodes can be psuedo-nodes, meaning that their real meaning is unknown
                 * to a Parser. Psuedo nodes contain the locale it was detected in, and its
                 * symbol. Typically, the ID and its flags are internally reserved for such a Node.
                 *
                 * @fn isPseudo
                 * @return 'true' if the flags and ID are psuedo-worthy, returns 'false' otherwise.
                 */
                Q_INVOKABLE inline const bool isPseudo() const {
                    return Lexical::Cache::isPseudo (m_lxdt);
                }

                /**
                 * @brief Prints out a QString representing this Node.
                 * @fn toString
                 * @param p_verbosity The verbosity of the printing of the Node.
                 */
                Q_INVOKABLE const string toString ( const FormatVerbosity& = FULL ) const;

                /**
                 * @brief Prints out a QString representing a specific Node.
                 * @fn toString
                 * @param p_node The Node to be printed.
                 * @param p_verbosity The verbosity of the printing of the Node.
                 */
                static const string toString ( const Node* , const FormatVerbosity& = FULL );

                /**
                 * @brief Prints out an entire NodeVector using a specific FormatVerbosity.
                 * @fn toString
                 * @param p_ndVtr The vector to be printed out.
                 * @param p_verbosity The verbosity for each Node in the NodeVector to be printed with.
                 */
                static const string toString ( const NodeList& , const FormatVerbosity& = FULL );

                /**
                 * @brief Determines if a specific Node exists.
                 * @fn exists
                 * @param p_lcl The locale of the potential Node.
                 * @param p_sym The symbol of the potential Node.
                 */
                static const bool exists ( const string&, const string& );

                /**
                 * @brief Obtains a Node from a specific locale with a specific symbol.
                 * @fn obtain
                 * @param p_lcl The locale of the potential Node.
                 * @param p_sym The symbol of the potential Node.
                 * @return A Node if the Node exists, or NULL if it doesn't.
                 */
                static Node* obtain ( const string&, const string& );

                /**
                 * @brief Creates a new Node based on a Lexical::Data.
                 * @fn create
                 * @param p_data The Lexical::Data to generate a Node with.
                 * @return The Node generated from the Lexical::Data or NULL if the operation failed.
                 * @warning This method may fail quietly, if the Lexical::Cache is unable to write the Lexical::Data to disk.
                 *          In a future implementation that uses error handling via exceptions, you'll be able to catch such a failure.
                 */
                static Node* create( const Lexical::Data& );

                /**
                 * @brief Generates a Node from a QString.
                 * @fn fromString
                 * @param p_str The QString to generate the Node from.
                 * @see obtain()
                 * @deprecated Use the obtain() method to obtain a Node from a QString.
                 * @obsolete This method will be removed in a later micro-version of WntrLing.
                 */
                static Node* fromString ( const string& );

                /**
                 * @brief Builds a psuedo-Node with the specified locale and symbol.
                 * @fn buildPseudo
                 * @param p_lcl The locale to use.
                 * @param p_sym The symbol of the Node.
                 * @return A Node representing the psuedo-Node, or NULL if no pseudo-Nodes could be formed.
                 * @todo Check if this Node exists. If it does, return that instead of forming a psuedo-Node.
                 */
                static Node* buildPseudo ( const string&, const string& );

                /**
                 * @brief Creates a Node based on a specific Lexical::Data.
                 * @fn form
                 * @param p_lxdt The Lexical::Data representing the internal data of the Node.
                 * @return A new Node with @var p_lxdt at its core.
                 * @see Node(Lexical::Data p_lxdt)
                 */
                static Node* form ( Lexical::Data );

                /**
                 * @brief Splits a Node into many Nodes.
                 *
                 * As mentioned in the preface, Nodes can represent a single symbol
                 * but contain many different meanings. This method returns a list of
                 * Nodes in a NodeList that each have only one of the flags defined in
                 * their originating Node.
                 *
                 * @fn expand
                 * @param p_node The Node to split.
                 * @return A NodeList with a Node for each flag defind in p_node.
                 */
                static NodeList expand ( const Node* );

                /**
                 * @brief Equality operator.
                 * @fn operator==
                 * @param p_nd
                 * @return bool
                 */
                bool operator== (const Node& p_nd) {
                    return this->id () == p_nd.id () &&
                           this->m_lxdt.locale () == this->m_lxdt.locale ();
                }

        };

        /**
         * @brief Represents the syntactical binding of two Node objects by a pre-defined relationship.
         *
         * The binding object that describes the relationship between two nodes can be found
         * in this class. The @c Link object demonstrates the syntactic connection between two
         * nodes. Links are considered to be the <i>predicate</i>, in <a href="http://www.w3.org/TR/REC-rdf-syntax">RDF</a>
         * syntax, and they represent a relationship between two Node objects (as shown below).
         *
         * @dot
            digraph L {
                    graph [layout=dot rankdir=TB]

                    {Node1 Node2} -> Link -> Relationship
            }
         * @enddot
         *
         * @class Link syntax.hpp "include/wntr/ling/syntax.hpp"
         * @internal
         */
        class Link : public QObject {
            friend class Meaning;
            Q_OBJECT
            Q_PROPERTY(const Node* Source READ source)
            Q_PROPERTY(const Node* Destination READ destination)
            Q_PROPERTY(const string Locale READ locale)
            Q_PROPERTY(const string Flags READ flags)
            Q_PROPERTY(const string Value READ toString)
            Q_PROPERTY(const int Level READ level)

            public:
                /**
                 * @brief Generates a Link from two Nodes, binding flags and a locale setting.
                 *
                 * The link formed would look something like this:
                 *
                 * @dot
                    digraph L {
                            graph [layout=dot rankdir=TB]

                            {SourceNode DestinationNode} -> Link -> {Locale Flags}
                    }
                 * @enddot
                 *
                 * @fn form
                 * @param p_node1 The source Node.
                 * @param p_node2 The destination Node.
                 * @param p_flag The flags defining the relationship between the source Node and the destination Node.
                 * @param p_lcl The locale of the link.
                 * @warning The locale property may become obsolete in order to ensure that we're able to support translations.
                 */
                static Link* form ( const Node* , const Node* , const string&, const string& );

                /**
                 * @brief Forms a link from a QString.
                 * @fn fromString
                 * @param p_str The QString representing a Link.
                 */
                static Link* fromString ( const string& );

                /**
                 * @brief Obtains the source Node.
                 * @fn source
                 * @return A Node representing the source of the Link.
                 */
                Q_INVOKABLE inline const Node* source() const { return m_src; }

                /**
                 * @brief Obtains the destination Node.
                 * @fn destination
                 * @return A Node representing the destination of the Link.
                 */
                Q_INVOKABLE inline const Node* destination() const { return m_dst; }

                /**
                 * @brief
                 *
                 * @fn level
                 * @return const int
                 */
                Q_INVOKABLE inline const int level() const { return m_lvl; }

                /**
                 * @brief Obtains the locale of the Link.
                 * @fn locale
                 * @return The specified locale of the Link.
                 * @deprecated This method, as long with the concept of locale for Links, are phasing out.
                 * @obsolete
                 */
                Q_INVOKABLE inline const string locale() const { return m_lcl; }

                /**
                 * @brief Obtains the flags of the Link.
                 * @fn flags
                 * @return The flags describing the relationship of this Link.
                 */
                Q_INVOKABLE inline const string flags() const { return m_flgs; }

                /**
                 * @brief Obtains a QString that represents this Link.
                 * @fn toString
                 */
                Q_INVOKABLE const string toString() const;

                /**
                 * @brief Null constructor.
                 * @fn Link
                 */
                Link() : m_lcl(""), m_flgs(""), m_src(NULL), m_dst(NULL), m_lvl(0) { }

                /**
                 * @brief Copy constructor.
                 * @fn Link
                 * @param p_lnk The Link to be copied.
                 */
                Link( const Link& p_lnk ) : m_src(p_lnk.m_src), m_dst(p_lnk.m_dst), m_flgs(p_lnk.m_flgs), m_lcl(p_lnk.m_lcl), m_lvl(0) { }

            protected:
                /**
                 * @brief Initialization constructor.
                 * @fn Link
                 * @param p_src The source Node.
                 * @param p_dst The destination Node.
                 * @param p_flgs The relationship described by the flags of the source Node's locale.
                 * @param p_lcl The locale of the flag.
                 */
                Link ( const Node* p_src, const Node* p_dst, const string& p_flgs, const string& p_lcl ) :
                        m_src ( p_src ),m_dst ( p_dst ), m_flgs ( p_flgs ), m_lcl ( p_lcl ), m_lvl(0) { }

            private:
                mutable int m_lvl;
                const Node* m_src;
                const Node* m_dst;
                const string m_flgs;
                const string m_lcl;
        };

        /**
         * @brief Debug utility function.
         * @fn operator <<
         * @param QDebug
         * @param p_node The Node to be printed to the stream.
         */
        QDebug operator<<(QDebug , const Node *);

        /**
         * @brief Debug utility function.
         * @fn operator <<
         * @param QDebug
         * @param p_link The Link to be printed to the stream.
         */
        QDebug operator<<(QDebug , const Link *);
    }
}

Q_DECLARE_METATYPE(Wintermute::Linguistics::Link)
Q_DECLARE_METATYPE(Wintermute::Linguistics::Node)

#endif	/* __SYNTAX_HPP */

// kate: indent-mode cstyle; space-indent on; indent-width 4;
