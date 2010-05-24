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
qs = Message.objects.filter("pk", 1);
if (qs.size() != 1) fail("Wrong number of messages");
m2 = qs.at(0)
if (m2.text != "some message") fail("Wrong message text");

/* quit */
quit();
