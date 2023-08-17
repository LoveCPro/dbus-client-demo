
int route_get_dbus_data(DBusConnection *conn, struct list_head *route_s)
{
    DBusMessage *msg;
    DBusMessageIter args, dict, var;
    DBusMessageIter all_r_arr, one_r_entr;
    DBusPendingCall *pending;
    DBusError      error;

    struct route_value *r_value = NULL;
    int type = 0;
    int  val = 0;
    int ret = 0;

    const char *append_iface = DBUS_ROUTE_REQUEST_NAME;
    dbus_error_init(&error);

    /*创建一个函数调用信息
    *一个method call消息包含：远端进程的bus name，方法名字，方法的参数，远端进程中object path，可选的接口名字
    *                                       terget   object   interface   method name
    */
    msg = dbus_message_new_method_call(DBUS_ROUTE_TARGET, DBUS_ROUTE_OBJECT, DBUS_ROUTE_INTERFACE,
            METHOD_GET_ROUTE);

    if (msg == NULL) {
        ERR("create method(%s) call message failed.", DBUS_ROUTE_OBJECT);
        return -1;
    }


    //发送消息  并获得reply的handle
    if (!dbus_connection_send_with_reply(conn, msg, &pending, -1)) {
        ERR("send with reply failed:no memory.func=%s.", __FUNCTION__);
        dbus_message_unref(msg);
        return -1;
    }

    if (NULL == pending) {
        ERR("send with reply failed: pending is NULL.func=%s.", __FUNCTION__);
        dbus_message_unref(msg);
        return -1;
    }

    //使用dbus_connection_flush（）进行阻塞，直到所有传出消息都写入底层传输（如套接字）
    dbus_connection_flush(conn);
    //free massage
    dbus_message_unref(msg);

    //block until we recieve a reply
    dbus_pending_call_block(pending);
    // get the reply message
    msg = dbus_pending_call_steal_reply(pending);

    if (NULL == msg) {
        ERR("send with reply failed:no memory.func=%s.", __FUNCTION__);
        return -1;
    }

    // free the pending message handle
    dbus_pending_call_unref(pending);

    // read the parameters
    if (!dbus_message_iter_init(msg, &all_r_arr)) {
        fprintf(stderr, "Message has no arguments!\n");
        return  -1;
    }

    type = dbus_message_iter_get_arg_type(&all_r_arr);
    printf("type %d %s\n", type, __FUNCTION__);



    if (type != DBUS_TYPE_ARRAY) {
        printf("get type is not DBUS_TYPE_ARRAY, line=%d\n", __LINE__);
    }


    //get 子迭代器
    dbus_message_iter_recurse(&all_r_arr, &one_r_entr);

    printf("line=%d\n", __LINE__);

    while ((type = dbus_message_iter_get_arg_type(&one_r_entr)) !=
        DBUS_TYPE_INVALID) {
        char *tmp = NULL;
        DBusMessageIter  one_r_arr, obj_iter, str_iter;
        DBusMessageIter entry, arr, dict;


        if (type != DBUS_TYPE_DICT_ENTRY) {
            printf("get type is DBUS_TYPE_DICT_ENTRY, line=%d\n", __LINE__);
            return FALSE;
        }

        //get object path 
        dbus_message_iter_recurse(&one_r_entr, &obj_iter);
        dbus_message_iter_get_basic(&obj_iter, &tmp);
        printf("obj: %s\n", tmp);

        arr = obj_iter;
        dbus_message_iter_next(&arr);
        dbus_message_iter_recurse(&arr, &entry);


        dbus_message_iter_recurse(&entry, &str_iter);
        type = dbus_message_iter_get_arg_type(&str_iter);
        printf("get type %d line=%d\n", type, __LINE__);
        dbus_message_iter_get_basic(&str_iter, &tmp);
        printf("str: %s\n", tmp);

		
        one_r_arr = str_iter;
        dbus_message_iter_next(&one_r_arr);

        dbus_message_iter_recurse(&one_r_arr, &dict);

        while ((type = dbus_message_iter_get_arg_type(&dict)) !=
            DBUS_TYPE_INVALID) {
            DBusMessageIter  key_entry;
            char *r_key = NULL;
            char *param = NULL;

            if (type != DBUS_TYPE_DICT_ENTRY) {
                printf("get type is %d,  not  DBUS_TYPE_DICT_ENTRY, line=%d\n", type, __LINE__);
                break;
            }

            dbus_message_iter_recurse(&dict, &key_entry);
            type = dbus_message_iter_get_arg_type(&key_entry);

            if (DBUS_TYPE_STRING != type) {
                printf("get type is %d,  not  DBUS_TYPE_STRING, line=%d\n", type, __LINE__);
                break;
            }

            dbus_message_iter_get_basic(&key_entry, &r_key);
            printf("get key:%s\n", r_key);


            dbus_message_iter_next(&key_entry);

            type = dbus_message_iter_get_arg_type(&key_entry);

            if (type != DBUS_TYPE_VARIANT) {
                printf("type is %d,not DBUS_TYPE_VARIANT,  line=%d\n", type, __LINE__);
                return FALSE;
            }

            dbus_message_iter_recurse(&key_entry, &var);
            type = dbus_message_iter_get_arg_type(&var);
            printf("type is %d, line=%d\n", type, __LINE__);

            if (DBUS_TYPE_STRING == type) {
                printf("type is DBUS_TYPE_STRING\n");

                dbus_message_iter_get_basic(&var, &param);
                printf("get var:%s\n", param);

               /* get all val , do some thing*/

            } else if (DBUS_TYPE_UINT32 == type) {
                printf("type is DBUS_TYPE_UINT32\n");
                dbus_message_iter_get_basic(&var, &val);
                
                /*do some thing*/
            }

            dbus_message_iter_next(&dict);
        }
		
        dbus_message_iter_next(&one_r_entr);
    }


    // free reply and close connection
    dbus_message_unref(msg);

    if (dbus_bus_release_name(conn, DBUS_ROUTE_REQUEST_NAME, &error) == -1) {
        fprintf (stderr, "Error in dbus_bus_release_name\n");
        return ret;
    }

    return ret;
}
