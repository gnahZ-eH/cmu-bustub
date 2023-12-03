#include "primer/trie.h"
#include <memory>
#include <string_view>
#include <utility>
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
  if (root_ == nullptr) {
    copy_trie = Trie();
    std::map<char, std::shared_ptr<const TrieNode>> empty_map;
    auto new_node = std::make_shared<TrieNode>();
    new_node->children_ = empty_map;
    copy_trie.root_ = std::move(new_node);

    auto cursor = copy_trie.root_;
    for (int i = 0; i < static_cast<int>(key.size() - 1); i++) {
      char c = key[i];
      auto children = cursor -> children_;

      if (children.find(c) == children.end()) {
        // Not find, create a new node
        std::map<char, std::shared_ptr<const TrieNode>> empty_map;
        auto new_node = std::make_shared<TrieNode>();
        new_node->children_ = empty_map;
        children[c] = std::move(new_node);
      }

      cursor = children[c];
    }

    char last_c = key[key.size() - 1];
    auto children = cursor -> children_;
    
    auto new_value_node = std::make_shared<TrieNodeWithValue<T>>(std::make_shared<T>(std::move(value)));
    children[last_c] = std::move(new_value_node);
    return copy_trie;
  }

  copy_trie = Trie(root_->Clone());
  auto copy_cursor = copy_trie.root_;
  auto cursor = root_;

  for (int i = 0; i < static_cast<int>(key.size() - 1); i++) {
    char c = key[i];
    auto children = cursor -> children_;
    auto copy_children = copy_cursor -> children_;

    if (children.find(c) == children.end()) {
      // Not find, create a new node
      std::map<char, std::shared_ptr<const TrieNode>> empty_map;
      auto new_node = std::make_shared<TrieNode>();
      new_node->children_ = empty_map;
      copy_children[c] = std::move(new_node);
    } else {
      // Find the node, need to create a copy if existing one
      auto copy_node = children[c]->Clone();
      copy_children[c] = std::move(copy_node);
    }

    cursor = children[c];
    copy_cursor = copy_children[c];
  }

  char last_c = key[key.size() - 1];
  auto children = cursor -> children_;
  auto copy_children = copy_cursor -> children_;
  if (children.find(last_c) == children.end()) {
    // Not find, create a new value node
    auto new_node = std::make_shared<TrieNodeWithValue<T>>(std::make_shared<T>(std::move(value)));
    copy_children[last_c] = std::move(new_node);
  } else {
    // Find the node, update its value
    auto copy_node = children[last_c]->Clone();
    auto value_node = dynamic_cast<TrieNodeWithValue<T> *>(copy_node.get());
    value_node->value_ = std::make_shared<T>(std::move(value));
  }
  return copy_trie;
}

auto Trie::Remove(std::string_view key) const -> Trie {
  throw NotImplementedException("Trie::Remove is not implemented.");

  // You should walk through the trie and remove nodes if necessary. If the node doesn't contain a value any more,
  // you should convert it to `TrieNode`. If a node doesn't have children any more, you should remove it.
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
