#include <gio/gio.h>
#include <glib.h>
#include <glib-object.h>
#include <gio/gdbusconnection.h>

static int XXX_add_object_with_dbus(char *value) 
{
	DBusMessage* msg;
	DBusMessageIter args;
	DBusError err;
	DBusPendingCall* pending;
	int type;
	char *param = "";

	DBusConnection *conn = dbus_init(DBUS_XXX_REQUEST_NAME);
	
	// create a new method call and check for errors
	msg = dbus_message_new_method_call(DBUS_TARGET, // target for the method call
									   OBJ_XXX, // object to call on
									   INTERFACE_xxx, // interface to call on
									   "AddObject"); // method name
	if (NULL == msg) { 
	   fprintf(stderr, "Message Null\n");
	   return DBUS_ERROR; 
	}

	// send message and get a handle for a reply
	if (!dbus_connection_send_with_reply (conn, msg, &pending, -1)) { // -1 is default timeout
	  fprintf(stderr, "Out Of Memory!\n"); 
	  return DBUS_ERROR; 
	}
	if (NULL == pending) { 
	  fprintf(stderr, "Pending Call Null\n"); 
	  return DBUS_ERROR; 
	}
	dbus_connection_flush(conn);
	printf("Request Sent\n");

	dbus_message_unref(msg);	// free message
	dbus_pending_call_block(pending);  // block until we recieve a reply
	msg = dbus_pending_call_steal_reply(pending); // get the reply message
	if (NULL == msg) {
	  fprintf(stderr, "Reply Null\n"); 
	  return DBUS_ERROR; 
	}
	
	dbus_pending_call_unref(pending);// free the pending message handle
	if (!dbus_message_iter_init(msg, &args)){
	  fprintf(stderr, "Message has no arguments!\n"); 
	  return DBUS_ERROR;
	} // read the parameters

	type = dbus_message_iter_get_arg_type(&args);
	printf("type %d %s\n",type,__FUNCTION__);

	dbus_message_iter_get_basic(&args, &param);
	strncpy(value,param,DBUS_OBJECT_PATH_LEN);
	
	printf("%s:Got Reply object path %s \n",__FUNCTION__,param);

	dbus_message_unref(msg);   // free reply and close connection

	return DBUS_OK; 
}

static int xxx_set_attr_with_dbus(char *object_path) 
{
	GDBusConnection *bus;
	GDBusMessage *message;
	GVariantBuilder builder;
	GVariant *body;
	GError *error = NULL;
	
	printf("Calling remote method with %s \n", object_path);
	bus = g_bus_get_sync (G_BUS_TYPE_SYSTEM, NULL, NULL);
	printf("%s %d\n",__FUNCTION__,__LINE__);
	
	message = g_dbus_message_new_method_call (DBUS_TARGET,
                                              object_path, // object to call on
									   		  "com.ctc.igd1.Properties", // interface to call on
									          "SetMulti");

	g_variant_builder_init (&builder, G_VARIANT_TYPE_ARRAY);	
	g_variant_builder_add (&builder, "{sv}","Enable", g_variant_new_boolean(TRUE));
    /*SET OTHER  Attributes*/


	/* send message */
	char *ifname = XXX_IFNAME;
	body = g_variant_new ("(sa{sv})", ifname, &builder);
	g_dbus_message_set_body (message, body);


    if(!g_dbus_connection_send_message (bus,
                                    message,
                                    G_DBUS_SEND_MESSAGE_FLAGS_NONE,
                                    NULL,
                                    &error)){
		printf("%s %d:err=%s\n",__FUNCTION__,__LINE__, error->message);
		g_error_free(error);
	}
	
    g_object_unref (message);
	return DBUS_OK;
}