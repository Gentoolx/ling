/**
 * @file    meanings.cpp
 * @author  Jacky Alcine <jackyalcine@gmail.com>
 * @date    August 20, 2011 8:58 PM
 * @license GPL3
 *
 * @legalese
 * Copyright (c) SII 2010 - 2011
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

#include "syntax.hpp"
#include "parser.hpp"
#include "meanings.hpp"

namespace Wintermute {
    namespace Linguistics {
        Meaning::Meaning() { }

        Meaning::Meaning(const LinkVector &p_lnkVtr) : m_lnkVtr(p_lnkVtr) { __init(); }

        Meaning::Meaning(const Meaning &p_meaning) : m_lnkVtr(p_meaning.m_lnkVtr) { __init(); }

        /// @todo Raise an exception and crash if this word is foreign OR have it be registered under a psuedo word OR implement a means of creating a new RuleSet.
        const Meaning* Meaning::form ( LinkVector* p_lnkVtr, const NodeVector& p_ndVtr ) {
            cout << endl << setw(20) << setfill('=') << " " << endl;
            NodeVector::ConstIterator l_ndItr = p_ndVtr.begin ();
            NodeVector l_ndVtr;
            QStringList* l_hideList = NULL;
            bool l_hideOther = false, l_hideThis = false;

            qDebug() << "(ling) [Meaning] Current nodes being parsed: '" << endl
                     << p_ndVtr << endl;

            if (p_ndVtr.size () != 1){
                for ( ; l_ndItr != p_ndVtr.end (); l_ndItr++ ) {
                    const Node *l_nd, *l_nd2;
                    if ( p_ndVtr.size () == 2 ) {
                        l_nd = p_ndVtr.front ();
                        l_nd2 = p_ndVtr.back ();
                    } else {
                        if ( ( l_ndItr + 1 ) != p_ndVtr.end () ) {
                            l_nd =  ( * ( l_ndItr ) );
                            l_nd2 = ( * ( l_ndItr + 1 ) );
                        } else break;
                    }

                    if (l_hideList){
                        const QString l_k(l_nd->toString (Node::EXTRA).c_str ());
                        bool l_b = false;
                        foreach (const QString l_s, *l_hideList)
                            if (l_k.contains (l_s)) l_b = true;

                        if (!l_b) {
                            qDebug() << "(ling) [Meaning] *** This node broke the filter; may appear on next round." << endl;
                            l_hideList = NULL;
                            l_hideThis = false;
                        }
                        else {
                            l_hideThis = true;
                            qDebug() << "(ling) [Meaning] *** Node won't appear in next round due to filter '" << l_hideList->join (",") << "'." << endl;
                        }
                    }

                    qDebug() << "(ling) [Meaning] Current node: " << l_nd;
                    const Binding* l_bnd = Binding::obtain ( *l_nd,*l_nd2 );
                    const Link* l_lnk;
                    if ( l_bnd ) {
                        l_lnk = l_bnd->bind ( *l_nd,*l_nd2 );
                        p_lnkVtr->push_back ( const_cast<Link*>(l_lnk) );

                        QString l_hide = l_bnd->getAttrValue("hide");
                        QString l_hideNext = l_bnd->getAttrValue("hideNext");
                        QString l_skipWord = l_bnd->getAttrValue("skipWord");
                        QString l_hideFilter = l_bnd->getAttrValue ("hideFilter");

                        l_hide = (l_hide.isEmpty () || l_hide.isNull ()) ? "no" : l_hide;
                        l_hideNext = (l_hideNext.isEmpty () || l_hideNext.isNull ()) ? "no" : l_hideNext;
                        l_skipWord = (l_skipWord.isEmpty () || l_skipWord.isNull ()) ? "yes" : l_skipWord;

                        // Attribute 'hide': Prevents this node (source node) from appearing on the next round of parsing. (default = 'no')
                        if ( !l_hideThis && !l_hideOther && l_hide == "no" )
                            l_ndVtr.push_back ( const_cast<Node*>( dynamic_cast<const Node*> ( l_lnk->source () ) ) );
                        else {
                            qDebug() << "(ling) [Meaning] *** Hid '" << l_lnk->source () << "' from appearing on the next pass of parsing.";
                        }

                        // Attribute 'hideNext': Prevents the next node (destination node) from appearing on its next round of parsing (2 rounds from now) (default = 'no')
                        if ( l_hideNext == "yes" ) {
                            l_hideOther = true;
                            qDebug() << "(ling) [Meaning] *** Hid '" << l_lnk->destination () << "' from appearing on the next pass of parsing (2 rounds from now).";
                        } else l_hideOther = false;

                        // Attribute 'skipWord': Doesn't allow the destination node to have a chance at being parsed. (default = yes)
                        if ( l_skipWord == "yes" )
                            l_ndItr++;
                        else {
                            qDebug() << "(ling) [Meaning] *** Skipping prevented for word-symbol '" << l_lnk->destination () << "'; will be parsed on next round.";
                        }


                        // Attribute 'hideFilter': Hides a set of words from appearing on the next round of parsing; a wrapper for the 'hide' attribute. (default = "")
                        if ( l_hideFilter.length () != 0 ) {
                            QStringList *l_e = new QStringList;

                            if ( l_hideFilter.contains ( "," ) ) {
                                QStringList d = l_hideFilter.split ( "," );
                                foreach ( const QString q, d )
                                l_e->append ( q );
                            } else l_e->append ( l_hideFilter );

                            l_hideList = l_e;
                            qDebug() << "(ling) [Meaning] *** Hiding any nodes that falls into the regex" << l_hideList->join (" 'or' ") << "on the next round.";
                        }
    #if 0
                        qDebug() << "(ling) [Meaning] Flags> hide: (" << l_hide
                             << ") hideThis: ("   << ((l_hideThis == true) ? "yes" : "no")
                             << ") hideOther: ("  << ((l_hideOther == true) ? "yes" : "no")
                             << ") hideNext: ("   << l_hideNext
                             << ") hideFilter: (" << l_hideFilter
                             << ") skipWord: ("   << l_skipWord
                             << ") hideList: ("   << ((l_hideList == NULL) ? "NULL" : "*") << ")"
                             << endl << "Link sig: " << l_lnk->toString ().c_str ()<< endl;
    #endif

                    } else {
                        //qWarning() << "(ling) [Meaning] Linking failed ... horribly." << endl;
                        l_lnk = NULL;
                    }
    #if 0
                    qDebug() << endl << "(ling) [Meaning] Nodes to be queued:";
                    for (int i = 0; i < l_ndVtr.size (); i++)
                        qDebug() << l_ndVtr.at (i);
                    qDebug() << "'" << endl << endl;
    #endif

                }
                qDebug() << "(ling) [Meaning] Formed" << p_lnkVtr->size () << "links with" << l_ndVtr.size () << "nodes left to parse.";
            }

            if ( !p_lnkVtr->empty () ) {
                if ( ! ( p_lnkVtr->size () >= 1) || l_ndVtr.size () > 0 )
                    return Meaning::form ( &*p_lnkVtr, l_ndVtr );
                else
                    return new Meaning ( *p_lnkVtr );
            } else
                return NULL;
        }

        const Link* Meaning::base () const { return m_lnkVtr.back (); }

        const LinkVector* Meaning::siblings () const { return &m_lnkVtr; }

        void Meaning::__init(){
            unique(m_lnkVtr.begin (),m_lnkVtr.end ());
            m_ontoMap.clear();

            foreach (Link* l_lnk, m_lnkVtr){
                Node* l_nd = const_cast<Node*>(l_lnk->source ());
                m_ontoMap.insert(l_nd,l_lnk);
            }

            qDebug() << "(ling) [Meaning] Encapsulates" << m_ontoMap.uniqueKeys ();
        }

        const LinkVector Meaning::isLinkedTo(const Node& p_nd) const {
            LinkVector l_lnkVtr;

            foreach (Link* l_lnk, m_lnkVtr){
                if (l_lnk->source () == &p_nd)
                    l_lnkVtr << l_lnk;
            }

            return l_lnkVtr;
        }

        const LinkVector Meaning::isLinkedBy(const Node& p_nd) const {
            LinkVector l_lnkVtr;

            foreach (Link* l_lnk, m_lnkVtr){
                if (l_lnk->destination () == &p_nd)
                    l_lnkVtr << l_lnk;
            }

            return l_lnkVtr;
        }

        const QString Meaning::toText () const {
            qDebug() << "(ling) [Meaning] Has" << m_lnkVtr.size () << "link(s).";

            foreach (const Link* l_lnk, m_lnkVtr)
                qDebug() << "(ling) [Meaning]" << l_lnk;

            return QString::null;
        }

        Meaning::~Meaning () { }

    }
}
