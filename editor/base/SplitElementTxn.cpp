/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * The contents of this file are subject to the Netscape Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/NPL/
 *
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is Netscape
 * Communications Corporation.  Portions created by Netscape are
 * Copyright (C) 1998-1999 Netscape Communications Corporation. All
 * Rights Reserved.
 *
 * Contributor(s): 
 */

#include "SplitElementTxn.h"
#include "nsEditor.h"
#include "nsIDOMNode.h"
#include "nsISelection.h"
#include "nsIDOMCharacterData.h"

#ifdef NS_DEBUG
static PRBool gNoisy = PR_FALSE;
#else
static const PRBool gNoisy = PR_FALSE;
#endif


// note that aEditor is not refcounted
SplitElementTxn::SplitElementTxn()
  : EditTxn()
{
}

NS_IMETHODIMP SplitElementTxn::Init(nsEditor   *aEditor,
                                    nsIDOMNode *aNode,
                                    PRInt32     aOffset)
{
  NS_ASSERTION(aEditor && aNode, "bad args");
  if (!aEditor || !aNode) { return NS_ERROR_NOT_INITIALIZED; }

  mEditor = aEditor;
  mExistingRightNode = do_QueryInterface(aNode);
  mOffset = aOffset;
  return NS_OK;
}

SplitElementTxn::~SplitElementTxn()
{
}

NS_IMETHODIMP SplitElementTxn::DoTransaction(void)
{
  if (gNoisy) { printf("%p Do Split of node %p offset %d\n", this, mExistingRightNode.get(), mOffset); }
  NS_ASSERTION(mExistingRightNode && mEditor, "bad state");
  if (!mExistingRightNode || !mEditor) { return NS_ERROR_NOT_INITIALIZED; }

  // create a new node
  nsresult result = mExistingRightNode->CloneNode(PR_FALSE, getter_AddRefs(mNewLeftNode));
  NS_ASSERTION(((NS_SUCCEEDED(result)) && (mNewLeftNode)), "could not create element.");
  if (NS_FAILED(result)) return result;
  if (!mNewLeftNode) return NS_ERROR_NULL_POINTER;
  mEditor->MarkNodeDirty(mExistingRightNode);


  if (gNoisy) { printf("  created left node = %p\n", mNewLeftNode.get()); }
  // get the parent node
  result = mExistingRightNode->GetParentNode(getter_AddRefs(mParent));
  if (NS_FAILED(result)) return result;
  if (!mParent) return NS_ERROR_NULL_POINTER;

  // insert the new node
  result = mEditor->SplitNodeImpl(mExistingRightNode, mOffset, mNewLeftNode, mParent);
  if (NS_SUCCEEDED(result) && mNewLeftNode)
  {
    nsCOMPtr<nsISelection>selection;
    mEditor->GetSelection(getter_AddRefs(selection));
    if (NS_FAILED(result)) return result;
    if (!selection) return NS_ERROR_NULL_POINTER;
    result = selection->Collapse(mNewLeftNode, mOffset);
  }
  else {
    result = NS_ERROR_NOT_IMPLEMENTED;
  }
  return result;
}

NS_IMETHODIMP SplitElementTxn::UndoTransaction(void)
{
  if (gNoisy) { 
    printf("%p Undo Split of existing node %p and new node %p offset %d\n", 
           this, mExistingRightNode.get(), mNewLeftNode.get(), mOffset); 
  }
  NS_ASSERTION(mEditor && mExistingRightNode && mNewLeftNode && mParent, "bad state");
  if (!mEditor || !mExistingRightNode || !mNewLeftNode || !mParent) {
    return NS_ERROR_NOT_INITIALIZED;
  }

  // this assumes Do inserted the new node in front of the prior existing node
  nsresult result = mEditor->JoinNodesImpl(mExistingRightNode, mNewLeftNode, mParent, PR_FALSE);
  if (gNoisy) 
  { 
    printf("** after join left child node %p into right node %p\n", mNewLeftNode.get(), mExistingRightNode.get());
    if (gNoisy) {mEditor->DebugDumpContent(); } // DEBUG
  }
  if (NS_SUCCEEDED(result))
  {
    if (gNoisy) { printf("  left node = %p removed\n", mNewLeftNode.get()); }
  }
  return result;
}

/* redo cannot simply resplit the right node, because subsequent transactions
 * on the redo stack may depend on the left node existing in its previous state.
 */
NS_IMETHODIMP SplitElementTxn::RedoTransaction(void)
{
  NS_ASSERTION(mEditor && mExistingRightNode && mNewLeftNode && mParent, "bad state");
  if (!mEditor || !mExistingRightNode || !mNewLeftNode || !mParent) {
    return NS_ERROR_NOT_INITIALIZED;
  }
  if (gNoisy) { 
    printf("%p Redo Split of existing node %p and new node %p offset %d\n", 
           this, mExistingRightNode.get(), mNewLeftNode.get(), mOffset); 
    if (gNoisy) {mEditor->DebugDumpContent(); } // DEBUG
  }
  nsresult result;
  nsCOMPtr<nsIDOMNode>resultNode;
  // first, massage the existing node so it is in its post-split state
  nsCOMPtr<nsIDOMCharacterData>rightNodeAsText;
  rightNodeAsText = do_QueryInterface(mExistingRightNode);
  if (rightNodeAsText)
  {
    result = rightNodeAsText->DeleteData(0, mOffset);
    if (gNoisy) 
    { 
      printf("** after delete of text in right text node %p offset %d\n", rightNodeAsText.get(), mOffset);
      mEditor->DebugDumpContent();  // DEBUG
    }
  }
  else
  {
    nsCOMPtr<nsIDOMNode>child;
    nsCOMPtr<nsIDOMNode>nextSibling;
    result = mExistingRightNode->GetFirstChild(getter_AddRefs(child));
    PRInt32 i;
    for (i=0; i<mOffset; i++)
    {
      if (NS_FAILED(result)) {return result;}
      if (!child) {return NS_ERROR_NULL_POINTER;}
      child->GetNextSibling(getter_AddRefs(nextSibling));
      result = mExistingRightNode->RemoveChild(child, getter_AddRefs(resultNode));
      if (NS_SUCCEEDED(result)) 
      {
        result = mNewLeftNode->AppendChild(child, getter_AddRefs(resultNode));
        if (gNoisy) 
        { 
          printf("** move child node %p from right node %p to left node %p\n", child.get(), mExistingRightNode.get(), mNewLeftNode.get());
          if (gNoisy) {mEditor->DebugDumpContent(); } // DEBUG
        }
      }
      child = do_QueryInterface(nextSibling);
    }
  }
  // second, re-insert the left node into the tree 
  result = mParent->InsertBefore(mNewLeftNode, mExistingRightNode, getter_AddRefs(resultNode));
  if (gNoisy) 
  { 
    printf("** reinsert left child node %p before right node %p\n", mNewLeftNode.get(), mExistingRightNode.get());
    if (gNoisy) {mEditor->DebugDumpContent(); } // DEBUG
  }
  return result;
}


NS_IMETHODIMP SplitElementTxn::Merge(nsITransaction *aTransaction, PRBool *aDidMerge)
{
  if (nsnull!=aDidMerge)
    *aDidMerge=PR_FALSE;
  return NS_OK;
}

NS_IMETHODIMP SplitElementTxn::GetTxnDescription(nsAWritableString& aString)
{
  aString.Assign(NS_LITERAL_STRING("SplitElementTxn"));
  return NS_OK;
}

NS_IMETHODIMP SplitElementTxn::GetNewNode(nsIDOMNode **aNewNode)
{
  if (!aNewNode)
    return NS_ERROR_NULL_POINTER;
  if (!mNewLeftNode)
    return NS_ERROR_NOT_INITIALIZED;
  *aNewNode = mNewLeftNode;
  NS_ADDREF(*aNewNode);
  return NS_OK;
}
