#include "primer/trie.h"
#include <memory>
#include <string_view>
#include <utility>
#include <vector>
#include "common/exception.h"

namespace bustub {

template <class T>
auto Trie::Get(std::string_view key) const -> const T * {
  //throw NotImplementedException("Trie::Get is not implemented.");

  // You should walk through the trie to find the node corresponding to the key. If the node doesn't exist, return
  // nullptr. After you find the node, you should use `dynamic_cast` to cast it to `const TrieNodeWithValue<T> *`. If
  // dynamic_cast returns `nullptr`, it means the type of the value is mismatched, and you should return nullptr.
  // Otherwise, return the value.
  if (root_ == nullptr) {
    return nullptr;
  }

  std::shared_ptr<const TrieNode> cur = root_;

  for (char c : key) {
    auto children = cur->children_;
    if (children.find(c) == children.end()) {
      return nullptr;
    }
    cur = children[c];
  }

  if (!(cur->is_value_node_)) {
    return nullptr;
  }

  auto value_node = dynamic_cast<const TrieNodeWithValue<T> *>(cur.get());
  if (value_node == nullptr) {
    return nullptr;
  }
  return value_node->value_.get();
}

template <class T>
auto Trie::Put(std::string_view key, T value) const -> Trie {
  // Note that `T` might be a non-copyable type. Always use `std::move` when creating `shared_ptr` on that value.
  //throw NotImplementedException("Trie::Put is not implemented.");

  // You should walk through the trie and create new nodes if necessary. If the node corresponding to the key already
  // exists, you should create a new `TrieNodeWithValue`.
  Trie copy_trie;
  std::shared_ptr<T> shared_value = std::make_shared<T>(std::move(value));
  if (root_ == nullptr) {
    copy_trie = Trie();
    std::map<char, std::shared_ptr<const TrieNode>> empty_map;
    auto new_node = std::make_shared<TrieNode>();
    new_node->children_ = empty_map;
    copy_trie.root_ = std::move(new_node);

    auto cursor = copy_trie.root_;
    for (int i = 0; i < static_cast<int>(key.size() - 1); i++) {
      char c = key[i];
      if (cursor -> children_.find(c) == cursor -> children_.end()) {
        // Not find, create a new node
        std::map<char, std::shared_ptr<const TrieNode>> empty_map;
        auto new_node = std::make_shared<TrieNode>();
        new_node->children_ = empty_map;
        const_cast<TrieNode *>(cursor.get())->children_[c] = std::move(new_node);
      }
      cursor = cursor -> children_.at(c);
    }

    auto new_value_node = std::make_shared<TrieNodeWithValue<T>>(shared_value);
    if (!key.empty()) {
      char last_c = key[key.size() - 1];
      
      const_cast<TrieNode *>(cursor.get())->children_[last_c] = std::move(new_value_node);
    } else {
      copy_trie.root_ = new_value_node;
    }
    return copy_trie;
  }

  copy_trie = Trie(root_->Clone());
  auto copy_cursor = copy_trie.root_;
  auto cursor = root_;

  for (int i = 0; i < static_cast<int>(key.size() - 1); i++) {
    char c = key[i];

    if (cursor == nullptr || cursor -> children_.find(c) == cursor -> children_.end()) {
      // Not find, create a new node
      std::map<char, std::shared_ptr<const TrieNode>> empty_map;
      auto new_node = std::make_shared<TrieNode>();
      new_node->children_ = empty_map;
      const_cast<TrieNode *>(copy_cursor.get())->children_[c] = std::move(new_node);
    } else {
      // Find the node, need to create a copy if existing one
      auto copy_node = const_cast<TrieNode *>(cursor.get())->children_[c]->Clone();
      const_cast<TrieNode *>(copy_cursor.get())->children_[c] = std::move(copy_node);
    }

    cursor = (cursor == nullptr || cursor->children_.find(c) == cursor->children_.end()) ? nullptr : cursor -> children_.at(c);
    copy_cursor = copy_cursor -> children_.at(c);
  }

  if (!key.empty()) {
    char last_c = key[key.size() - 1];
    if (cursor == nullptr || cursor -> children_.find(last_c) == cursor -> children_.end()) {
      // Not find, create a new value node
      auto new_node = std::make_shared<TrieNodeWithValue<T>>(shared_value);
      const_cast<TrieNode *>(copy_cursor.get())->children_[last_c] = std::move(new_node);
    } else {
      // Find the node, update its value
      auto copy_children = const_cast<TrieNode *>(cursor.get())->children_[last_c]->children_;
      auto copy_node = std::make_shared<const TrieNodeWithValue<T>>(copy_children, shared_value);
      // hezhang TODO: value_node?
      const_cast<TrieNode *>(copy_cursor.get())->children_[last_c] = std::move(copy_node);
    }
  } else {
    auto copy_children = const_cast<TrieNode *>(cursor.get())->children_;
    auto copy_node = std::make_shared<const TrieNodeWithValue<T>>(copy_children, shared_value);
    copy_trie.root_ = copy_node;
  }
  return copy_trie;
}


auto Trie::Remove(std::string_view key) const -> Trie {
  Trie copy_trie;
  if (root_ == nullptr) {
    copy_trie = Trie();
    return copy_trie;
  }

  copy_trie = Trie(root_->Clone());
  if (key.empty()) {
    //copy_trie.root_ = std::make_shared<TrieNode>(copy_trie.root_->children_);
    if (copy_trie.root_->is_value_node_) {
      copy_trie.root_ = nullptr;
    }
    return copy_trie;
  }

  auto cur = copy_trie.root_;
  
  std::vector<std::shared_ptr<const TrieNode>> stack;
  stack.push_back(cur);
  for (int i = 0; i < static_cast<int>(key.size() - 1); i++) {
    char c = key[i];
    if (cur->children_.find(c) != cur->children_.end()) {
      stack.push_back(cur->children_.at(c));
      cur = cur->children_.at(c);
    } else {
      cur = nullptr;
      break;
    }
  }

  char last_char = key[key.size() - 1];
  if (cur == nullptr || cur->children_.find(last_char) == cur->children_.end()) {
    return copy_trie;
  }

  auto stack_size = stack.size();
  // find the leaf node, TrieNodeWithValue->TrieNode
  auto copy_children = const_cast<TrieNode *>(cur.get())->children_[last_char]->children_;
  if (!copy_children.empty()) {
    auto copy_node = std::make_shared<const TrieNode>(copy_children);
  
    const_cast<TrieNode *>(stack[stack_size - 1].get())->children_[last_char] = copy_node;
    return copy_trie;
  }

  // set leaf node to non-value node
  const_cast<TrieNode *>(cur.get())->children_[last_char] = std::make_shared<const TrieNode>();

  for (int i = stack_size - 1; i >= 0; i--) {
    auto child_node = const_cast<TrieNode *>(stack[i].get())->children_[key[i]];

    if (child_node->is_value_node_) {
      break;
    }

    if (child_node->children_.empty()) {
      const_cast<TrieNode *>(stack[i].get())->children_.erase(key[i]);
    }
  }
  if (copy_trie.root_->children_.empty()) {
    copy_trie.root_ = nullptr;
  }
  return copy_trie;
}

// Below are explicit instantiation of template functions.
//
// Generally people would write the implementation of template classes and functions in the header file. However, we
// separate the implementation into a .cpp file to make things clearer. In order to make the compiler know the
// implementation of the template functions, we need to explicitly instantiate them here, so that they can be picked up
// by the linker.

template auto Trie::Put(std::string_view key, uint32_t value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const uint32_t *;

template auto Trie::Put(std::string_view key, uint64_t value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const uint64_t *;

template auto Trie::Put(std::string_view key, std::string value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const std::string *;

// If your solution cannot compile for non-copy tests, you can remove the below lines to get partial score.

using Integer = std::unique_ptr<uint32_t>;

template auto Trie::Put(std::string_view key, Integer value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const Integer *;

template auto Trie::Put(std::string_view key, MoveBlocked value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const MoveBlocked *;

}  // namespace bustub
