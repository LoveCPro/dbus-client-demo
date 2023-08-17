static void get_interface_and_property(GVariant      *ifaces_and_properties)
{
	GVariantIter iter;
	const gchar *interface_name;
	GVariant *properties;
	GVariantIter property_iter;
	const gchar *property_name;
	GVariant *property_value;
	
	g_variant_iter_init (&iter, ifaces_and_properties);
	while (g_variant_iter_next (&iter,
	                          "{&s@a{sv}}",
	                          &interface_name,
	                          &properties))
	{
		printf("%s %d interface_name : %s\n",__FUNCTION__,__LINE__,interface_name);

		g_variant_iter_init (&property_iter, properties);
		while (g_variant_iter_next (&property_iter,
		                          "{&sv}",
		                          &property_name,
		                          &property_value))
		{
			if (!strcmp(property_name,"Enable")){
				printf("%s: %d \n",property_name,g_variant_get_boolean (property_value));	
			}
            /*set other  Attributes*/
			
			g_variant_unref (property_value);
		}
		g_variant_unref (properties);
	}
}

static int XXX_get_attr_with_dbus() 
{
	GDBusConnection *bus;
	GDBusMessage *message;
	GDBusMessage *method_reply_message;
	GError *error = NULL;
	GVariant *arg0;
	const gchar *object_path;
	GVariant *ifaces_and_properties;
	GVariantIter iter;

	printf("Calling remote method with %s \n", __FUNCTION__);
	bus = g_bus_get_sync (G_BUS_TYPE_SYSTEM, NULL, NULL);
	printf("%s %d\n",__FUNCTION__,__LINE__);
	
	message = g_dbus_message_new_method_call (DBUS_TARGET,
                                              XXX_OBJ, // object to call on
									   		  xxx_INTERFACE, // interface to call on
									          "GetManagedObjects");


	method_reply_message = g_dbus_connection_send_message_with_reply_sync (bus,
																		 message,
																		 G_DBUS_SEND_MESSAGE_FLAGS_NONE,
																		 -1,
																		 NULL, /* out_serial */
																		 NULL, /* cancellable */
																		 &error);
	if (method_reply_message == NULL)
	  goto out;

	if (g_dbus_message_get_message_type (method_reply_message) == G_DBUS_MESSAGE_TYPE_ERROR)
	{
		g_dbus_message_to_gerror (method_reply_message, &error);
		goto out;
	}

	arg0 = g_variant_get_child_value (g_dbus_message_get_body (method_reply_message), 0);
	g_variant_iter_init (&iter, arg0);
	while (g_variant_iter_next (&iter,
	                          "{&o@a{sa{sv}}}",
	                          &object_path,
	                          &ifaces_and_properties))
	{
		printf("%s %d path : %s\n",__FUNCTION__,__LINE__,object_path);
		get_interface_and_property(ifaces_and_properties);
		g_variant_unref (ifaces_and_properties);
	}
	g_variant_unref (arg0);

	
out:
	g_object_unref (message);
	g_object_unref (method_reply_message);

	printf("%s %d\n",__FUNCTION__,__LINE__);
	return DBUS_OK;
}
