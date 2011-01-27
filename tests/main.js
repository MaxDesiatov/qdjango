/*
 * QDjango
 * Copyright (C) 2010-2011 Bolloré telecom
 * See AUTHORS file for a full list of contributors.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

function fail(msg)
{
    print(msg);
    quit();
}

load("qdjango.test");
syncdb();

/* create a user */
u = new User();
u.username = "foouser";
u.password = "foopass";
u.save();

/* create a message */
m = new Message();
m.text = "some message";
m.user_id = u.pk;
m.save();

/* find message */
qs = Message.objects.filter({"pk": 1});
if (qs.size() != 1) fail("Wrong number of messages");
m2 = qs.at(0)
if (m2.text != "some message") fail("Wrong message text");

/* quit */
quit();
