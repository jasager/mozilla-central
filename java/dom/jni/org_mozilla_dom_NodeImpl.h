/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class org_mozilla_dom_NodeImpl */

#ifndef _Included_org_mozilla_dom_NodeImpl
#define _Included_org_mozilla_dom_NodeImpl
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     org_mozilla_dom_NodeImpl
 * Method:    XPCOM_equals
 * Signature: (Ljava/lang/Object;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_mozilla_dom_NodeImpl_XPCOM_1equals
  (JNIEnv *, jobject, jobject);

/*
 * Class:     org_mozilla_dom_NodeImpl
 * Method:    XPCOM_hashCode
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_org_mozilla_dom_NodeImpl_XPCOM_1hashCode
  (JNIEnv *, jobject);

/*
 * Class:     org_mozilla_dom_NodeImpl
 * Method:    addNativeEventListener
 * Signature: (Ljava/lang/String;Lorg/w3c/dom/events/EventListener;Z)J
 */
JNIEXPORT jlong JNICALL Java_org_mozilla_dom_NodeImpl_addNativeEventListener
  (JNIEnv *, jobject, jstring, jobject, jboolean);

/*
 * Class:     org_mozilla_dom_NodeImpl
 * Method:    appendChild
 * Signature: (Lorg/w3c/dom/Node;)Lorg/w3c/dom/Node;
 */
JNIEXPORT jobject JNICALL Java_org_mozilla_dom_NodeImpl_appendChild
  (JNIEnv *, jobject, jobject);

/*
 * Class:     org_mozilla_dom_NodeImpl
 * Method:    cloneNode
 * Signature: (Z)Lorg/w3c/dom/Node;
 */
JNIEXPORT jobject JNICALL Java_org_mozilla_dom_NodeImpl_cloneNode
  (JNIEnv *, jobject, jboolean);

/*
 * Class:     org_mozilla_dom_NodeImpl
 * Method:    finalize
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_mozilla_dom_NodeImpl_finalize
  (JNIEnv *, jobject);

/*
 * Class:     org_mozilla_dom_NodeImpl
 * Method:    getAttributes
 * Signature: ()Lorg/w3c/dom/NamedNodeMap;
 */
JNIEXPORT jobject JNICALL Java_org_mozilla_dom_NodeImpl_getAttributes
  (JNIEnv *, jobject);

/*
 * Class:     org_mozilla_dom_NodeImpl
 * Method:    getChildNodes
 * Signature: ()Lorg/w3c/dom/NodeList;
 */
JNIEXPORT jobject JNICALL Java_org_mozilla_dom_NodeImpl_getChildNodes
  (JNIEnv *, jobject);

/*
 * Class:     org_mozilla_dom_NodeImpl
 * Method:    getFirstChild
 * Signature: ()Lorg/w3c/dom/Node;
 */
JNIEXPORT jobject JNICALL Java_org_mozilla_dom_NodeImpl_getFirstChild
  (JNIEnv *, jobject);

/*
 * Class:     org_mozilla_dom_NodeImpl
 * Method:    getLastChild
 * Signature: ()Lorg/w3c/dom/Node;
 */
JNIEXPORT jobject JNICALL Java_org_mozilla_dom_NodeImpl_getLastChild
  (JNIEnv *, jobject);

/*
 * Class:     org_mozilla_dom_NodeImpl
 * Method:    getNextSibling
 * Signature: ()Lorg/w3c/dom/Node;
 */
JNIEXPORT jobject JNICALL Java_org_mozilla_dom_NodeImpl_getNextSibling
  (JNIEnv *, jobject);

/*
 * Class:     org_mozilla_dom_NodeImpl
 * Method:    getNodeName
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_mozilla_dom_NodeImpl_getNodeName
  (JNIEnv *, jobject);

/*
 * Class:     org_mozilla_dom_NodeImpl
 * Method:    getNodeType
 * Signature: ()S
 */
JNIEXPORT jshort JNICALL Java_org_mozilla_dom_NodeImpl_getNodeType
  (JNIEnv *, jobject);

/*
 * Class:     org_mozilla_dom_NodeImpl
 * Method:    getNodeValue
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_mozilla_dom_NodeImpl_getNodeValue
  (JNIEnv *, jobject);

/*
 * Class:     org_mozilla_dom_NodeImpl
 * Method:    getOwnerDocument
 * Signature: ()Lorg/w3c/dom/Document;
 */
JNIEXPORT jobject JNICALL Java_org_mozilla_dom_NodeImpl_getOwnerDocument
  (JNIEnv *, jobject);

/*
 * Class:     org_mozilla_dom_NodeImpl
 * Method:    getParentNode
 * Signature: ()Lorg/w3c/dom/Node;
 */
JNIEXPORT jobject JNICALL Java_org_mozilla_dom_NodeImpl_getParentNode
  (JNIEnv *, jobject);

/*
 * Class:     org_mozilla_dom_NodeImpl
 * Method:    getPreviousSibling
 * Signature: ()Lorg/w3c/dom/Node;
 */
JNIEXPORT jobject JNICALL Java_org_mozilla_dom_NodeImpl_getPreviousSibling
  (JNIEnv *, jobject);

/*
 * Class:     org_mozilla_dom_NodeImpl
 * Method:    hasChildNodes
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_org_mozilla_dom_NodeImpl_hasChildNodes
  (JNIEnv *, jobject);

/*
 * Class:     org_mozilla_dom_NodeImpl
 * Method:    insertBefore
 * Signature: (Lorg/w3c/dom/Node;Lorg/w3c/dom/Node;)Lorg/w3c/dom/Node;
 */
JNIEXPORT jobject JNICALL Java_org_mozilla_dom_NodeImpl_insertBefore
  (JNIEnv *, jobject, jobject, jobject);

/*
 * Class:     org_mozilla_dom_NodeImpl
 * Method:    removeChild
 * Signature: (Lorg/w3c/dom/Node;)Lorg/w3c/dom/Node;
 */
JNIEXPORT jobject JNICALL Java_org_mozilla_dom_NodeImpl_removeChild
  (JNIEnv *, jobject, jobject);

/*
 * Class:     org_mozilla_dom_NodeImpl
 * Method:    removeNativeEventListener
 * Signature: (Ljava/lang/String;JZ)V
 */
JNIEXPORT void JNICALL Java_org_mozilla_dom_NodeImpl_removeNativeEventListener
  (JNIEnv *, jobject, jstring, jlong, jboolean);

/*
 * Class:     org_mozilla_dom_NodeImpl
 * Method:    replaceChild
 * Signature: (Lorg/w3c/dom/Node;Lorg/w3c/dom/Node;)Lorg/w3c/dom/Node;
 */
JNIEXPORT jobject JNICALL Java_org_mozilla_dom_NodeImpl_replaceChild
  (JNIEnv *, jobject, jobject, jobject);

/*
 * Class:     org_mozilla_dom_NodeImpl
 * Method:    setNodeValue
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_mozilla_dom_NodeImpl_setNodeValue
  (JNIEnv *, jobject, jstring);

#ifdef __cplusplus
}
#endif
#endif
