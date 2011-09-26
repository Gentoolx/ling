/**
 * @file    adaptors.hpp
 * @author  Wintermute Developers <wintermute-devel@lists.launchpad.net>
 * @created 9/6/2011
 *
 *
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Wintermute Linguistics is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Wintermute Linguistics; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#ifndef WNTRLING_ADAPTORS_HPP
#define WNTRLING_ADAPTORS_HPP

#include <wntr/adaptors.hpp>

namespace Wintermute {
    namespace Linguistics {
        class SystemAdaptor : public Adaptor {
            Q_OBJECT
            Q_CLASSINFO("D-Bus Interface","org.thesii.Wintermute.Linguistics.System")

            public:
                explicit SystemAdaptor();

            signals:
                void initialized();
                void deinitialized();

            public slots:
                virtual void quit(const QDBusMessage&) const;
        };
    }
}

#endif // ADAPTORS_HPP
