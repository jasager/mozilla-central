/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim: set ts=8 sts=4 et sw=4 tw=99:
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * JavaScript Debugging support - Value and Property support
 */

#include "jsd.h"
#include "jsapi.h"
#include "jsfriendapi.h"
#include "jswrapper.h"
#include "nsCxPusher.h"

using mozilla::AutoSafeJSContext;

#ifdef DEBUG
void JSD_ASSERT_VALID_VALUE(JSDValue* jsdval)
{
    JS_ASSERT(jsdval);
    JS_ASSERT(jsdval->nref > 0);
    if(!JS_CLIST_IS_EMPTY(&jsdval->props))
    {
        JS_ASSERT(CHECK_BIT_FLAG(jsdval->flags, GOT_PROPS));
        JS_ASSERT(!JSVAL_IS_PRIMITIVE(jsdval->val));
    }

    if(jsdval->proto)
    {
        JS_ASSERT(CHECK_BIT_FLAG(jsdval->flags, GOT_PROTO));
        JS_ASSERT(jsdval->proto->nref > 0);
    }
    if(jsdval->parent)
    {
        JS_ASSERT(CHECK_BIT_FLAG(jsdval->flags, GOT_PARENT));
        JS_ASSERT(jsdval->parent->nref > 0);
    }
    if(jsdval->ctor)
    {
        JS_ASSERT(CHECK_BIT_FLAG(jsdval->flags, GOT_CTOR));
        JS_ASSERT(jsdval->ctor->nref > 0);
    }
}

void JSD_ASSERT_VALID_PROPERTY(JSDProperty* jsdprop)
{
    JS_ASSERT(jsdprop);
    JS_ASSERT(jsdprop->name);
    JS_ASSERT(jsdprop->name->nref > 0);
    JS_ASSERT(jsdprop->val);
    JS_ASSERT(jsdprop->val->nref > 0);
    if(jsdprop->alias)
        JS_ASSERT(jsdprop->alias->nref > 0);
}
#endif


JSBool
jsd_IsValueObject(JSDContext* jsdc, JSDValue* jsdval)
{
    return !JSVAL_IS_PRIMITIVE(jsdval->val) || JSVAL_IS_NULL(jsdval->val);
}

JSBool
jsd_IsValueNumber(JSDContext* jsdc, JSDValue* jsdval)
{
    return JSVAL_IS_NUMBER(jsdval->val);
}

JSBool
jsd_IsValueInt(JSDContext* jsdc, JSDValue* jsdval)
{
    return JSVAL_IS_INT(jsdval->val);
}

JSBool
jsd_IsValueDouble(JSDContext* jsdc, JSDValue* jsdval)
{
    return JSVAL_IS_DOUBLE(jsdval->val);
}

JSBool
jsd_IsValueString(JSDContext* jsdc, JSDValue* jsdval)
{
    return JSVAL_IS_STRING(jsdval->val);
}

JSBool
jsd_IsValueBoolean(JSDContext* jsdc, JSDValue* jsdval)
{
    return JSVAL_IS_BOOLEAN(jsdval->val);
}

JSBool
jsd_IsValueNull(JSDContext* jsdc, JSDValue* jsdval)
{
    return JSVAL_IS_NULL(jsdval->val);
}

JSBool
jsd_IsValueVoid(JSDContext* jsdc, JSDValue* jsdval)
{
    return JSVAL_IS_VOID(jsdval->val);
}

JSBool
jsd_IsValuePrimitive(JSDContext* jsdc, JSDValue* jsdval)
{
    return JSVAL_IS_PRIMITIVE(jsdval->val);
}

JSBool
jsd_IsValueFunction(JSDContext* jsdc, JSDValue* jsdval)
{
    AutoSafeJSContext cx; // NB: Actually unused.
    return !JSVAL_IS_PRIMITIVE(jsdval->val) &&
           JS_ObjectIsCallable(cx, JSVAL_TO_OBJECT(jsdval->val));
}

JSBool
jsd_IsValueNative(JSDContext* jsdc, JSDValue* jsdval)
{
    AutoSafeJSContext cx;
    JS::RootedFunction fun(cx);

    if(jsd_IsValueFunction(jsdc, jsdval))
    {
        JSAutoCompartment ac(cx, JSVAL_TO_OBJECT(jsdval->val));
        AutoSaveExceptionState as(cx);
        JSBool ok = false;
        fun = JSD_GetValueFunction(jsdc, jsdval);
        if(fun)
            ok = JS_GetFunctionScript(cx, fun) ? false : true;
        JS_ASSERT(fun);
        return ok;
    }
    return !JSVAL_IS_PRIMITIVE(jsdval->val);
}

/***************************************************************************/

JSBool
jsd_GetValueBoolean(JSDContext* jsdc, JSDValue* jsdval)
{
    jsval val = jsdval->val;
    if(!JSVAL_IS_BOOLEAN(val))
        return false;
    return JSVAL_TO_BOOLEAN(val);
}

int32_t
jsd_GetValueInt(JSDContext* jsdc, JSDValue* jsdval)
{
    jsval val = jsdval->val;
    if(!JSVAL_IS_INT(val))
        return 0;
    return JSVAL_TO_INT(val);
}

double
jsd_GetValueDouble(JSDContext* jsdc, JSDValue* jsdval)
{
    if(!JSVAL_IS_DOUBLE(jsdval->val))
        return 0;
    return JSVAL_TO_DOUBLE(jsdval->val);
}

JSString*
jsd_GetValueString(JSDContext* jsdc, JSDValue* jsdval)
{
    AutoSafeJSContext cx;
    JS::RootedValue stringval(cx);
    JS::RootedString string(cx);
    JS::RootedObject scopeObj(cx);

    if(jsdval->string)
        return jsdval->string;

    /* Reuse the string without copying or re-rooting it */
    if(JSVAL_IS_STRING(jsdval->val)) {
        jsdval->string = JSVAL_TO_STRING(jsdval->val);
        return jsdval->string;
    }

    /* Objects call JS_ValueToString in their own compartment. */
    scopeObj = !JSVAL_IS_PRIMITIVE(jsdval->val) ? JSVAL_TO_OBJECT(jsdval->val) : jsdc->glob;
    {
        JSAutoCompartment ac(cx, scopeObj);
        AutoSaveExceptionState as(cx);
        string = JS_ValueToString(cx, jsdval->val);
    }

    JSAutoCompartment ac2(cx, jsdc->glob);
    if(string) {
        stringval = STRING_TO_JSVAL(string);
    }
    if(!string || !JS_WrapValue(cx, stringval.address())) {
        return NULL;
    }

    jsdval->string = JSVAL_TO_STRING(stringval);
    if(!JS_AddNamedStringRoot(cx, &jsdval->string, "ValueString"))
        jsdval->string = NULL;

    return jsdval->string;
}

JSString*
jsd_GetValueFunctionId(JSDContext* jsdc, JSDValue* jsdval)
{
    AutoSafeJSContext cx;
    JS::RootedFunction fun(cx);

    if(!jsdval->funName && jsd_IsValueFunction(jsdc, jsdval))
    {
        JSAutoCompartment ac(cx, JSVAL_TO_OBJECT(jsdval->val));
        AutoSaveExceptionState as(cx);
        fun = JSD_GetValueFunction(jsdc, jsdval);
        if(!fun)
            return NULL;
        jsdval->funName = JS_GetFunctionId(fun);

        /* For compatibility we return "anonymous", not an empty string here. */
        if (!jsdval->funName)
            jsdval->funName = JS_GetAnonymousString(jsdc->jsrt);
    }
    return jsdval->funName;
}

/***************************************************************************/

/*
 * Create a new JSD value referring to a jsval. Copy string values into the
 * JSD compartment. Leave all other GCTHINGs in their native compartments
 * and access them through cross-compartment calls.
 */
JSDValue*
jsd_NewValue(JSDContext* jsdc, jsval value)
{
    AutoSafeJSContext cx;
    JS::RootedValue val(cx, value);
    JSDValue* jsdval;

    if(!(jsdval = (JSDValue*) calloc(1, sizeof(JSDValue))))
        return NULL;

    if(JSVAL_IS_GCTHING(val))
    {
        JSBool ok;
        JSAutoCompartment ac(cx, jsdc->glob);

        ok = JS_AddNamedValueRoot(cx, &jsdval->val, "JSDValue");
        if(ok && JSVAL_IS_STRING(val)) {
            if(!JS_WrapValue(cx, val.address())) {
                ok = false;
            }
        }

        if(!ok)
        {
            free(jsdval);
            return NULL;
        }
    }
    jsdval->val  = val;
    jsdval->nref = 1;
    JS_INIT_CLIST(&jsdval->props);

    return jsdval;
}

void
jsd_DropValue(JSDContext* jsdc, JSDValue* jsdval)
{
    JS_ASSERT(jsdval->nref > 0);
    if(0 == --jsdval->nref)
    {
        jsd_RefreshValue(jsdc, jsdval);
        if(JSVAL_IS_GCTHING(jsdval->val))
        {
            AutoSafeJSContext cx;
            JSAutoCompartment ac(cx, jsdc->glob);
            JS_RemoveValueRoot(cx, &jsdval->val);
        }
        free(jsdval);
    }
}

jsval
jsd_GetValueWrappedJSVal(JSDContext* jsdc, JSDValue* jsdval)
{
    AutoSafeJSContext cx;
    JS::RootedObject obj(cx);
    JS::RootedValue val(cx, jsdval->val);
    if (!JSVAL_IS_PRIMITIVE(val)) {
        JSAutoCompartment ac(cx, JSVAL_TO_OBJECT(val));
        obj = JS_ObjectToOuterObject(cx, JSVAL_TO_OBJECT(val));
        if (!obj)
        {
            JS_ClearPendingException(cx);
            val = JSVAL_NULL;
        }
        else
            val = OBJECT_TO_JSVAL(obj);
    }
    
    return val;
}

static JSDProperty* _newProperty(JSDContext* jsdc, JSPropertyDesc* pd,
                                 unsigned additionalFlags)
{
    JSDProperty* jsdprop;

    if(!(jsdprop = (JSDProperty*) calloc(1, sizeof(JSDProperty))))
        return NULL;

    JS_INIT_CLIST(&jsdprop->links);
    jsdprop->nref = 1;
    jsdprop->flags = pd->flags | additionalFlags;

    if(!(jsdprop->name = jsd_NewValue(jsdc, pd->id)))
        goto new_prop_fail;

    if(!(jsdprop->val = jsd_NewValue(jsdc, pd->value)))
        goto new_prop_fail;

    if((jsdprop->flags & JSDPD_ALIAS) &&
       !(jsdprop->alias = jsd_NewValue(jsdc, pd->alias)))
        goto new_prop_fail;

    return jsdprop;
new_prop_fail:
    jsd_DropProperty(jsdc, jsdprop);
    return NULL;
}

static void _freeProps(JSDContext* jsdc, JSDValue* jsdval)
{
    JSDProperty* jsdprop;

    while(jsdprop = (JSDProperty*)jsdval->props.next,
          jsdprop != (JSDProperty*)&jsdval->props)
    {
        JS_REMOVE_AND_INIT_LINK(&jsdprop->links);
        jsd_DropProperty(jsdc, jsdprop);
    }
    JS_ASSERT(JS_CLIST_IS_EMPTY(&jsdval->props));
    CLEAR_BIT_FLAG(jsdval->flags, GOT_PROPS);
}

static JSBool _buildProps(JSDContext* jsdc, JSDValue* jsdval)
{
    AutoSafeJSContext cx;
    JS::RootedObject obj(cx);
    JSPropertyDescArray pda;
    unsigned i;

    JS_ASSERT(JS_CLIST_IS_EMPTY(&jsdval->props));
    JS_ASSERT(!(CHECK_BIT_FLAG(jsdval->flags, GOT_PROPS)));
    JS_ASSERT(!JSVAL_IS_PRIMITIVE(jsdval->val));

    if(JSVAL_IS_PRIMITIVE(jsdval->val))
        return false;

    obj = JSVAL_TO_OBJECT(jsdval->val);

    JSAutoCompartment ac(cx, obj);

    if(!JS_GetPropertyDescArray(cx, obj, &pda))
    {
        return false;
    }

    for(i = 0; i < pda.length; i++)
    {
        JSDProperty* prop = _newProperty(jsdc, &pda.array[i], 0);
        if(!prop)
        {
            _freeProps(jsdc, jsdval);
            break;
        }
        JS_APPEND_LINK(&prop->links, &jsdval->props);
    }
    JS_PutPropertyDescArray(cx, &pda);
    SET_BIT_FLAG(jsdval->flags, GOT_PROPS);
    return !JS_CLIST_IS_EMPTY(&jsdval->props);
}

#undef  DROP_CLEAR_VALUE
#define DROP_CLEAR_VALUE(jsdc, x) if(x){jsd_DropValue(jsdc,x); x = NULL;}

void
jsd_RefreshValue(JSDContext* jsdc, JSDValue* jsdval)
{
    AutoSafeJSContext cx;
    if(jsdval->string)
    {
        /* if the jsval is a string, then we didn't need to root the string */
        if(!JSVAL_IS_STRING(jsdval->val))
        {
            JSAutoCompartment ac(cx, jsdc->glob);
            JS_RemoveStringRoot(cx, &jsdval->string);
        }
        jsdval->string = NULL;
    }

    jsdval->funName = NULL;
    jsdval->className = NULL;
    DROP_CLEAR_VALUE(jsdc, jsdval->proto);
    DROP_CLEAR_VALUE(jsdc, jsdval->parent);
    DROP_CLEAR_VALUE(jsdc, jsdval->ctor);
    _freeProps(jsdc, jsdval);
    jsdval->flags = 0;
}

/***************************************************************************/

unsigned
jsd_GetCountOfProperties(JSDContext* jsdc, JSDValue* jsdval)
{
    JSDProperty* jsdprop;
    unsigned count = 0;

    if(!(CHECK_BIT_FLAG(jsdval->flags, GOT_PROPS)))
        if(!_buildProps(jsdc, jsdval))
            return 0;

    for(jsdprop = (JSDProperty*)jsdval->props.next;
        jsdprop != (JSDProperty*)&jsdval->props;
        jsdprop = (JSDProperty*)jsdprop->links.next)
    {
        count++;
    }
    return count;
}

JSDProperty*
jsd_IterateProperties(JSDContext* jsdc, JSDValue* jsdval, JSDProperty **iterp)
{
    JSDProperty* jsdprop = *iterp;
    if(!(CHECK_BIT_FLAG(jsdval->flags, GOT_PROPS)))
    {
        JS_ASSERT(!jsdprop);
        if(!_buildProps(jsdc, jsdval))
            return NULL;
    }

    if(!jsdprop)
        jsdprop = (JSDProperty*)jsdval->props.next;
    if(jsdprop == (JSDProperty*)&jsdval->props)
        return NULL;
    *iterp = (JSDProperty*)jsdprop->links.next;

    JS_ASSERT(jsdprop);
    jsdprop->nref++;
    return jsdprop;
}

JSDProperty*
jsd_GetValueProperty(JSDContext* jsdc, JSDValue* jsdval, JSString* nameStr)
{
    AutoSafeJSContext cx;
    JSAutoCompartment acBase(cx, jsdc->glob);
    JSDProperty* jsdprop;
    JSDProperty* iter = NULL;
    JS::RootedObject obj(cx);
    JS::RootedString name(cx, nameStr);
    unsigned  attrs = 0;
    JSBool found;
    JSPropertyDesc pd;
    const jschar * nameChars;
    size_t nameLen;
    JS::RootedValue val(cx), nameval(cx);
    JS::RootedId nameid(cx);

    if(!jsd_IsValueObject(jsdc, jsdval))
        return NULL;

    /* If we already have the prop, then return it */
    while(NULL != (jsdprop = jsd_IterateProperties(jsdc, jsdval, &iter)))
    {
        JSString* propName = jsd_GetValueString(jsdc, jsdprop->name);
        if(propName) {
            int result;
            if (JS_CompareStrings(cx, propName, name, &result) && !result)
                return jsdprop;
        }
        JSD_DropProperty(jsdc, jsdprop);
    }
    /* Not found in property list, look it up explicitly */

    if(!(obj = JSVAL_TO_OBJECT(jsdval->val)))
        return NULL;

    if (!(nameChars = JS_GetStringCharsZAndLength(cx, name, &nameLen)))
        return NULL;

    {
        JSAutoCompartment ac(cx, obj);

        JS_GetUCPropertyAttributes(cx, obj, nameChars, nameLen, &attrs, &found);
        if (!found)
        {
            return NULL;
        }

        JS_ClearPendingException(cx);

        if(!JS_GetUCProperty(cx, obj, nameChars, nameLen, &val))
        {
            if (JS_IsExceptionPending(cx))
            {
                if (!JS_GetPendingException(cx, &pd.value))
                {
                    return NULL;
                }
                pd.flags = JSPD_EXCEPTION;
            }
            else
            {
                pd.flags = JSPD_ERROR;
                pd.value = JSVAL_VOID;
            }
        }
        else
        {
            pd.value = val;
        }
    }

    nameval = STRING_TO_JSVAL(name);
    if (!JS_ValueToId(cx, nameval, nameid.address()) ||
        !JS_IdToValue(cx, nameid, &pd.id)) {
        return NULL;
    }

    pd.spare = 0;
    pd.alias = JSVAL_NULL;
    pd.flags |= (attrs & JSPROP_ENUMERATE) ? JSPD_ENUMERATE : 0
        | (attrs & JSPROP_READONLY)  ? JSPD_READONLY  : 0
        | (attrs & JSPROP_PERMANENT) ? JSPD_PERMANENT : 0;

    return _newProperty(jsdc, &pd, JSDPD_HINTED);
}

/*
 * Retrieve a JSFunction* from a JSDValue*. This differs from
 * JS_ValueToFunction by fully unwrapping the object first.
 */
JSFunction*
jsd_GetValueFunction(JSDContext* jsdc, JSDValue* jsdval)
{
    AutoSafeJSContext cx;

    JS::RootedObject obj(cx);
    JS::RootedFunction fun(cx);

    if (JSVAL_IS_PRIMITIVE(jsdval->val))
        return NULL;

    obj = js::UncheckedUnwrap(JSVAL_TO_OBJECT(jsdval->val));
    JSAutoCompartment ac(cx, obj);
    fun = JS_ValueToFunction(cx, OBJECT_TO_JSVAL(obj));

    return fun;
}

JSDValue*
jsd_GetValuePrototype(JSDContext* jsdc, JSDValue* jsdval)
{
    AutoSafeJSContext cx;
    if(!(CHECK_BIT_FLAG(jsdval->flags, GOT_PROTO)))
    {
        JS::RootedObject obj(cx);
        JS::RootedObject proto(cx);
        JS_ASSERT(!jsdval->proto);
        SET_BIT_FLAG(jsdval->flags, GOT_PROTO);
        if(JSVAL_IS_PRIMITIVE(jsdval->val))
            return NULL;
        obj = JSVAL_TO_OBJECT(jsdval->val);
        if(!JS_GetPrototype(cx, obj, &proto))
            return NULL;
        if(!proto)
            return NULL;
        jsdval->proto = jsd_NewValue(jsdc, OBJECT_TO_JSVAL(proto));
    }
    if(jsdval->proto)
        jsdval->proto->nref++;
    return jsdval->proto;
}

JSDValue*
jsd_GetValueParent(JSDContext* jsdc, JSDValue* jsdval)
{
    if(!(CHECK_BIT_FLAG(jsdval->flags, GOT_PARENT)))
    {
        AutoSafeJSContext cx;
        JS::RootedObject obj(cx);
        JS::RootedObject parent(cx);
        JS_ASSERT(!jsdval->parent);
        SET_BIT_FLAG(jsdval->flags, GOT_PARENT);
        if(JSVAL_IS_PRIMITIVE(jsdval->val))
            return NULL;
        obj = JSVAL_TO_OBJECT(jsdval->val);
        {
            JSAutoCompartment ac(cx, obj);
            parent = JS_GetParentOrScopeChain(cx, obj);
        }
        if(!parent)
            return NULL;
        jsdval->parent = jsd_NewValue(jsdc, OBJECT_TO_JSVAL(parent));
    }
    if(jsdval->parent)
        jsdval->parent->nref++;
    return jsdval->parent;
}

JSDValue*
jsd_GetValueConstructor(JSDContext* jsdc, JSDValue* jsdval)
{
    if(!(CHECK_BIT_FLAG(jsdval->flags, GOT_CTOR)))
    {
        AutoSafeJSContext cx;
        JS::RootedObject obj(cx);
        JS::RootedObject proto(cx);
        JS::RootedObject ctor(cx);
        JS_ASSERT(!jsdval->ctor);
        SET_BIT_FLAG(jsdval->flags, GOT_CTOR);
        if(JSVAL_IS_PRIMITIVE(jsdval->val))
            return NULL;
        obj = JSVAL_TO_OBJECT(jsdval->val);
        if(!JS_GetPrototype(cx, obj, &proto))
            return NULL;
        if(!proto)
            return NULL;
        {
            JSAutoCompartment ac(cx, obj);
            ctor = JS_GetConstructor(cx, proto);
        }
        if(!ctor)
            return NULL;
        jsdval->ctor = jsd_NewValue(jsdc, OBJECT_TO_JSVAL(ctor));
    }
    if(jsdval->ctor)
        jsdval->ctor->nref++;
    return jsdval->ctor;
}

const char*
jsd_GetValueClassName(JSDContext* jsdc, JSDValue* jsdval)
{
    jsval val = jsdval->val;
    if(!jsdval->className && !JSVAL_IS_PRIMITIVE(val))
    {
        AutoSafeJSContext cx;
        JS::RootedObject obj(cx, JSVAL_TO_OBJECT(val));
        JSAutoCompartment ac(cx, obj);
        jsdval->className = JS_GetDebugClassName(obj);
    }
    return jsdval->className;
}

JSDScript*
jsd_GetScriptForValue(JSDContext* jsdc, JSDValue* jsdval)
{
    AutoSafeJSContext cx;
    JS::RootedValue val(cx, jsdval->val);
    JSFunction* fun = NULL;
    JS::RootedScript script(cx);
    JSDScript* jsdscript;

    if (!jsd_IsValueFunction(jsdc, jsdval))
        return NULL;

    {
        JSAutoCompartment ac(cx, JSVAL_TO_OBJECT(val));
        AutoSaveExceptionState as(cx);
        fun = JSD_GetValueFunction(jsdc, jsdval);
        if (fun)
            script = JS_GetFunctionScript(cx, fun);
    }

    if (!script)
        return NULL;

    JSD_LOCK_SCRIPTS(jsdc);
    jsdscript = jsd_FindJSDScript(jsdc, script);
    JSD_UNLOCK_SCRIPTS(jsdc);
    return jsdscript;
}


/***************************************************************************/
/***************************************************************************/

JSDValue*
jsd_GetPropertyName(JSDContext* jsdc, JSDProperty* jsdprop)
{
    jsdprop->name->nref++;
    return jsdprop->name;
}

JSDValue*
jsd_GetPropertyValue(JSDContext* jsdc, JSDProperty* jsdprop)
{
    jsdprop->val->nref++;
    return jsdprop->val;
}

JSDValue*
jsd_GetPropertyAlias(JSDContext* jsdc, JSDProperty* jsdprop)
{
    if(jsdprop->alias)
        jsdprop->alias->nref++;
    return jsdprop->alias;
}

unsigned
jsd_GetPropertyFlags(JSDContext* jsdc, JSDProperty* jsdprop)
{
    return jsdprop->flags;
}

void
jsd_DropProperty(JSDContext* jsdc, JSDProperty* jsdprop)
{
    JS_ASSERT(jsdprop->nref > 0);
    if(0 == --jsdprop->nref)
    {
        JS_ASSERT(JS_CLIST_IS_EMPTY(&jsdprop->links));
        DROP_CLEAR_VALUE(jsdc, jsdprop->val);
        DROP_CLEAR_VALUE(jsdc, jsdprop->name);
        DROP_CLEAR_VALUE(jsdc, jsdprop->alias);
        free(jsdprop);
    }
}
